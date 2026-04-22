# picasso

## Introduction

`picasso` is a PICA200 shader assembler, written in C++. The PICA200 is the GPU used by the Nintendo 3DS.

`picasso` comes with a manual `Manual.md` that explains the shader language. `example.vsh` is simple example that demonstrates it.

## Building

### Compiler frontend

A working C++ compiler with at least `C++11` Support is required as well as cmake

Building:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
### Library

Run the following commands

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release --toolchain /opt/devkitpro/cmake/3DS.cmake
cmake --build build
```

Installing is not supported (yet)

## Shout-outs

- **fincs** for making picasso
- **smea** for reverse-engineering the PICA200, writing documentation, working hard & making `aemstro_as.py` (the original homebrew PICA200 shader assembler)
- **neobrain** for making `nihstro-assemble`, whose syntax inspired that of `picasso` and whose usage of boost inspired me to make my own assembler without hefty dependencies.
