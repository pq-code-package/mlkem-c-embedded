# Dev Support

## Recommended Prerequisites

### [direnv](https://direnv.net/)

Enabling `direnv` would greatly enhance the development experience. Once enabled, all dependencies will be installed, and the shell will be augmented according to the specifications in [flake.nix](../flake.nix) whenever you enter the project directory from your shell. The initial setup might take a bit longer when entering the project directory for the first time.

`direnv` can be enabled or disabled using `direnv allow` and `direnv deny` respectively. If you're new to `direnv`, it is also recommended to [integrate it with your shell](https://direnv.net/docs/hook.html) first.

### Formatter and Linter
- `nixpkgs-fmt` and `deadnix` for `Nix`
- `shfmt` and `shellcheck` for shell script
- `astyle` for .c, .h files

### Tools

When used together with direnv, the tools should be automatically discoverable by the shell.

- [style](bin/style): Formatting, linting and spell checking all files
