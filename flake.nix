{
  description = "mlkem-c-embedded";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";

    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
  };

  outputs = inputs@{ flake-parts, nixpkgs, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [ ];
      systems = [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin" ];
      perSystem = { pkgs, system, inputs', ... }:
        {
          # Per-system attributes can be defined here. The self' and inputs'
          # module parameters provide easy access to attributes of the same
          # system.

          # Equivalent to  inputs'.nixpkgs.legacyPackages.hello;

          devShells.default = with pkgs; mkShellNoCC {
            packages = [
              direnv
              nix-direnv

              # formatter & linters
              nixpkgs-fmt
              shfmt
              codespell
              astyle # 3.4.10

              # build dependencies
              gcc-arm-embedded-13 # arm-gnu-toolchain-13.2.rel1
              openocd # 0.12.0
              python311Packages.pyserial # 3.5
            ];

            shellHook = ''
              export PATH=$PWD/dev-support/bin:$PATH
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

