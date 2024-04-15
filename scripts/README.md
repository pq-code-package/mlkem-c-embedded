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

`direnv` is required to install before used, for installation guide please refer to https://direnv.net/docs/installation.html

Enabling `direnv` would greatly enhance the development experience. Once enabled, all dependencies will be installed, and the shell will be augmented according to the specifications in [flake.nix](../flake.nix) whenever you enter the project directory from your shell. The initial setup might take a bit longer when entering the project directory for the first time.

If you're new to `direnv`,  you must hook it into your shell first (e.x. append `eval "$(direnv hook bash)"` in `~/.bashrc` for `bash`), for other shells please refer to [hook direnv into your shell](https://direnv.net/docs/hook.html).
After integrating `direnv` with your shell, it can be enabled or disabled using `direnv allow` and `direnv deny` respectively.

### Formatter and Linter
- `nixpkgs-fmt` for `Nix`
- `shfmt` and for shell script
- `astyle` for .c, .h files

### Scripts

When used together with direnv, the tools should be automatically discoverable by the shell.

- [format](format): Format all files
- [lint](ci/lint): Lint all files
- [test](ci/tests): Used for running func/speed/stack/nistkat tests
