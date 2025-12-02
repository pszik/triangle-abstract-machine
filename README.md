# Triangle Abstract Machine

![GitHub Release](https://img.shields.io/github/v/release/pszik/triangle-abstract-machine)
![GitHub Release](https://img.shields.io/github/v/release/pszik/triangle-abstract-machine?include_prereleases&filter=nightly*&display_name=tag&label=nightly)
![GitHub License](https://img.shields.io/github/license/pszik/triangle-abstract-machine)

This is an emulator for the Triangle Abstract Machine (TAM) defined by Watt and Brown.

It is invoked from the command line using the command `tam`. It expects a single
argument, which is the name of the binary file to load and run:

```
Usage: tam [OPTIONS] FILENAME

Options:
  -t,--trace        print the stack and allocated heap after each instruction
                    (optionally takes a number from 0-2 to indiciate how much
                    information to print each tick)
  -s,--step         press RETURN to advance after each instruction (only valid
                    if -t also given)
  -h,--help         print this help message
```

The `trace` option accepts one of three levels of detail:

- `-t 1` will print the mnemonic of each instruction as it is executed
- `-t 2` will print the mnemonic and also the values of SB, LB, ST, HT, HB, and CP after each instruction
- `-t 3` will print mnemonics, register values, and the full contents of the stack and
  allocated heap blocks

## Expected behaviour

TAM data memory uses 16-bit words and all operations will overflow or underflow
accordingly. In particular, adding or multiplying positive numbers will overflow
into the negative and _vice versa_.

An unexpected case is that negating -32768 (the smallest signed 16-bit number)
will still result in -32768.

## Building TAM

TAM is a CMake project. To build it yourself, clone the project and then
run the following commands:

```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

After this, the executable is found in `build/app/tam` on Unix, or `build\app\Release\tam`
on Windows if you build using Visual C++.
