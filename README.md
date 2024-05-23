[//]: # (SPDX-License-Identifier: CC-BY-4.0)

**MLKEM-C-EMBEDDED** is a collection of [MLKEM](https://doi.org/10.6028/NIST.FIPS.203.ipd) implementations optimized for embedded microcontrollers.
It is free software licensed under an Apache-2.0 license.

It originates from the [pqm4](https://github.com/mupq/pqm4) project, but there are some core differences:
- It is limited to ML-KEM allowing a much simpler build system and richer set of implementation options
- Once reaching v1.0, **MLKEM-C-EMBEDDED** can be relied on in production environments
- Portability is a high priority. We plan to use assembly only where absolutely necessary
- Speed is not the primary objective. We do not want to sacrifice maintainability and code size
- The scope extends beyond the Arm Cortex-M4 and we plan to have optimized code for many more (micro-)architectures

## Goals of MLKEM-C-EMBEDDED

The goals and features of a future **MLKEM-C-EMBEDDED** v1.0 release include:

- Provide production-grade portable C code that can be dropped into other projects
- Being permissibly licensed with all code being Apache-2.0 or CC0 licensed
- Tested against the official reference known-answer tests (KATs) and extended KATs (taken from another [PQCP](https://github.com/pq-code-package) project)
- A single source tree with no code duplication (e.g., for stack optimizations, parameter sets)
- Having a make-based build system allowing testing on various platforms (at least [stm32f4discovery](https://www.st.com/en/evaluation-tools/stm32f4discovery.html), [nucleo-l4r5zi](https://www.st.com/en/evaluation-tools/nucleo-l4r5zi.html), and [mps2-an386](https://developer.arm.com/documentation/dai0386/c)) relying on [libopencm3](https://github.com/libopencm3/libopencm3) and [openocd](https://openocd.org/)
- Supporting the [Arm GNU toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- Including speed and stack benchmarks
- Having a CI setup running tests and verifying testvectors using the [QEMU](https://www.qemu.org/) Arm system emulator
- Configurable stack optimizations matching the state-of-the-art
- Portable C code speed-optimized for 32-bit platforms

In the medium term, we hope to include:
 - Optional Cortex-M4 assembly for core building blocks
 - Dynamic parameter selection (for supporting multiple parameter sets with minimal code size)
 - Supporting both the Arm GNU toolchain and the Arm Compiler for Embedded

In the long term, possible extensions are
 - Support more 32-bit targets (e.g., RISC-V, Arm Cortex-M3. Arm Cortex-M7, Arm Cortex-M55, Arm Cortex-M85) with optional assembly
 - Automated checking for secret-dependent timing including data-dependent instruction timing such as `div` on most platforms or `umull` on Cortex-M3

## Current state

**MLKEM-C-EMBEDDED** is currently a work in progress and we do not recommend relying on it at this point.
**WE DO NOT CURRENTLY RECOMMEND RELYING ON THIS LIBRARY IN A PRODUCTION ENVIRONMENT OR TO PROTECT ANY SENSITIVE DATA.**
Once we have the first stable version, this notice will be removed

The current code is compatible with the [`standard` branch of the official MLKEM repository](https://github.com/pq-crystals/kyber/tree/standard).

## Call for contributors

We are actively seeking contributors who can help us build **MLKEM-C-EMBEDDED**.
If you are interested, please contact us, or volunteer for any of the open issues.

## Call for potential consumers

If you are a potential consumer of **MLKEM-C-EMBEDDED**, please reach out to us.
We're interested in hearing the way you are considering using **MLKEM-C-EMBEDDED** and could benefit from additional features.
If you have specific feature requests, please open an issue.

## Environment Setup

All the develop and build dependencies are specified in [flake.nix](flake.nix). 

- Setup with nix,
    - Running `nix develop` will execute a bash shell with the development environment specified in [flake.nix](flake.nix).
    - Alternatively, you can enable `direnv` by using `direnv allow`, allowing it to handle the environment setup for you.

- If your're not using nix, please ensure you have installed the same version as specified in [flake.nix](flake.nix).

For further details, please refer to [scripts/README.md](scripts/README.md)

## Running tests and benchmarks

The build system compiles tests and benchmarks for each mlkem parameter set on specified platform, supported platform can be found by running `tests --list-platforms`.
The PLATFORM configuration is optional, with the default platform set to `stm32f4discovery`.

For example,
- `make [PLATFORM=<PLATFORM_NAME>] [RNG=HAL|NOTRAND] test` assembles all binaries for functional tests.
- `make [PLATFORM=<PLATFORM_NAME>] [RNG=HAL|NOTRAND] speed` assembles all binaries for speed benchmarking
- `make [PLATFORM=<PLATFORM_NAME>] [RNG=HAL|NOTRAND] stack` assembles all binaries for stack benchmarking
- `make [PLATFORM=<PLATFORM_NAME>] RNG=NISTKAT nistkat` assembles all binaries for nistkat
- `make [PLATFORM=<PLATFORM_NAME>] (all)` assembles all the above targets for all parameter sets.

- `make emulate` run emulatation for the binary file on `QEMU`
- `make clean` cleans up intermediate artifacts

### Manual testing on board
After generating the specified hex files, you can flash it to the development board using `openocd`. 
For example,
```
    openocd -f hal/stm32f4discovery.cfg -c "program bin/mlkem768-test.hex verify reset exit"
```

To receive output from the develop board, you can, for example, use `pyserial-miniterm`: 
```
    pyserial-miniterm /dev/<tty_device> 38400
```

### Usage of the [tests script](scripts/tests)
Make sure to run `make clean` between running tests on QEMU or on board or running func/stack/speed and nistkat tests. In case of any inconsistencies, refer to the help command for the most up-to-date usage information

```
▶ tests --help
Usage: tests [OPTIONS] COMMAND [ARGS]...

Options:
  --list-platforms  List the supported platforms
  --help  Show this message and exit.

Commands:
  func     Run functional tests
  nistkat  Run nistkat tests
  run      Run for the specified platform and hex file without parsing the
           output
  speed    Run speed tests
  stack    Run stack tests
```

func/speed tests depends on the iteration parameter, which is passed to the tests in compile time, therefore it is preferred to build the binaries with the tests script

```
▶ tests func --help
Usage: tests func [OPTIONS] {stm32f4discovery|mps2-an386|nucleo-f767zi|mps2-an
                  500|nucleo-f207zg|mps2-an385}

Options:
  -v, --verbose             Show verbose output or not
  -u, --uart PATH           TTY serial device for UART, default to the 1st
                            serial device connected to your board or an empty
                            string
  -i, --iterations INTEGER  Number of tests  [default: 1]
  --help                    Show this message and exit.
```
