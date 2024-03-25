[//]: # (SPDX-License-Identifier: CC-BY-4.0)

# Dev Support

## Recommended Prerequisites

### [flake](https://nixos.wiki/wiki/Flakes)

As `flake` is still an experimental feature of nix, `--experimental-features 'nix-command flakes'` is needed for `nix` command.

Add the following to `~/.config/nix/nix.conf` or `/etc/nix/nix.conf` could avoid adding the option every time invoking `nix` command.
```
experimental-features = nix-command flakes
```


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
