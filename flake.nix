# SPDX-License-Identifier: Apache-2.0

{
  description = "mlkem-c-embedded";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";

    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
  };

  outputs = inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [ ];
      systems = [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin" ];
      perSystem = { pkgs, ... }:
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

          wrapShell = mkShell: attrs:
            mkShell (attrs // {
              shellHook = ''
                export PATH=$PWD/scripts:$PWD/scripts/ci:$PATH
              '';
            });
        in
        {
          devShells.default = wrapShell pkgs.mkShellNoCC {
            packages = core ++ arm-pkgs ++ builtins.attrValues {
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
        };
      flake = {
        # The usual flake attributes can be defined here, including system-
        # agnostic ones like nixosModule and system-enumerating ones, although
        # those are more easily expressed in perSystem.

      };
    };
}
