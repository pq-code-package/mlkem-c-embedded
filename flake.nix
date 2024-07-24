# SPDX-License-Identifier: Apache-2.0

{
  description = "mlkem-c-embedded";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";

    esp-dev = {
      url = "github:mirrexagon/nixpkgs-esp-dev?rev=86a2bbe01fe0258887de7396af2a5eb0e37ac3be";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
  };

  outputs = inputs@{ flake-parts, esp-dev, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [ ];
      systems = [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin" ];
      perSystem = { pkgs, system, ... }:
        let
          libopencm3 = pkgs.callPackage ./libopencm3.nix {
            targets = [ "stm32/f2" "stm32/f4" "stm32/f7" ];
          };
          mbed-os = pkgs.callPackage ./mbed-os.nix {
            targets = [ "TARGET_MPS2_M3" "TARGET_MPS2_M4" "TARGET_MPS2_M7" ];
          };

          core = builtins.attrValues {
            astyle = pkgs.astyle.overrideAttrs (old: rec {
              version = "3.4.13";
              src = pkgs.fetchurl {
                url = "mirror://sourceforge/${old.pname}/${old.pname}-${version}.tar.bz2";
                hash = "sha256-eKYQq9OelOD5E+nuXNoehbtizWM1U97LngDT2SAQGc4=";
              };
            });

            inherit (pkgs)
              # formatter & linters
              nixpkgs-fmt
              shfmt

              # build dependencies
              qemu# 8.2.4

              yq;

            inherit (pkgs.python311Packages)
              python
              black
              pyserial# 3.5
              click;
          };

          arm-pkgs = builtins.attrValues {
            libopencm3 = libopencm3;
            mbed-os = mbed-os;
            inherit (pkgs)
              gcc-arm-embedded-13; # arm-gnu-toolchain-13.2.rel1
          };

          riscv-pkgs = builtins.attrValues {
            esp-idf-lib = pkgs.callPackage ./esp-idf-lib.nix { };
          };

          wrapShell = mkShell: attrs:
            mkShell (attrs // {
              shellHook = ''
                export PATH=$PWD/scripts:$PWD/scripts/ci:$PATH
              '';
            });
        in
        {
          _module.args = {
            pkgs = import inputs.nixpkgs {
              inherit system;
              overlays = [ esp-dev.overlays.default ];
            };
          };

          devShells.default = wrapShell pkgs.mkShellNoCC {
            packages = core ++ arm-pkgs ++ riscv-pkgs ++ builtins.attrValues {
              inherit (pkgs)
                direnv
                nix-direnv

                # debug dependencies
                openocd; # 0.12.0
            };
            OPENCM3_DIR = ''${libopencm3}'';
            MBED_OS_DIR = ''${mbed-os}'';
          };

          devShells.ci = wrapShell pkgs.mkShellNoCC {
            packages = core ++ arm-pkgs;
            OPENCM3_DIR = ''${libopencm3}'';
            MBED_OS_DIR = ''${mbed-os}'';
          };

          devShells.ci-riscv = wrapShell pkgs.mkShellNoCC {
            packages = core ++ riscv-pkgs;
          };
        };
      flake = {
        # The usual flake attributes can be defined here, including system-
        # agnostic ones like nixosModule and system-enumerating ones, although
        # those are more easily expressed in perSystem.

      };
    };
}
