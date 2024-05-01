# SPDX-License-Identifier: Apache-2.0

{
  description = "mlkem-c-embedded";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";

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
            targets = [ "stm32/f4" ];
          };
          core = with pkgs; [
            # formatter & linters
            nixpkgs-fmt
            shfmt
            astyle # 3.4.10

            # build dependencies
            gcc-arm-embedded-13 # arm-gnu-toolchain-13.2.rel1
            python311
            qemu # 8.1.5
            libopencm3


            yq
            python311Packages.pyserial # 3.5
            python311Packages.click
          ];
        in
        {
          packages.default = libopencm3;
          devShells.default = with pkgs; mkShellNoCC {
            packages = core ++ [
              direnv
              nix-direnv

              # debug dependencies
              openocd # 0.12.0
            ];

            shellHook = ''
              export OPENCM3_DIR=${libopencm3}
              export PATH=$PWD/scripts:$PWD/scripts/ci:$PATH
            '';
          };

          devShells.ci = with pkgs; mkShellNoCC {
            packages = core;

            shellHook = ''
              export OPENCM3_DIR=${libopencm3}
              export PATH=$PWD/scripts:$PWD/scripts/ci:$PATH
            '';
          };
        };
      flake = {
        # The usual flake attributes can be defined here, including system-
        # agnostic ones like nixosModule and system-enumerating ones, although
        # those are more easily expressed in perSystem.

      };
    };
}
