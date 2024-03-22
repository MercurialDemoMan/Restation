# Playstation 1 Emulator with Higher Rendering Resolution

## Introduction

This repository contains a master's thesis focusing on development of a Playstation simulator.

## Repository structure

    .
    +--Data       - Example data, Measured results, etc.
    +--Literature - Publications, references, manuals, etc.
    +--Sources    - Root folder for the sources.
    +--Thesis     - Latex / MS Word sources of the thesis.
    +--Misc       - Other auxiliary materials.
    Readme.md     - Read me file


## Build instructions

## For Linux

Requirements:

 - CMake 3.5
 - gcc 12.0.0
 - g++ 12.0.0

Configure and compile using:

```bash
cd Sources/build
cmake ..
make
```

## For Windows

*Warning!* Do not try to compile this project using MSVC, because it sucks.
Instead, install a [WSL linux subsystem](https://learn.microsoft.com/en-us/windows/wsl/install) and setup a cross-compilation environment.
Depending on your linux distribution, you will need to install MinGW cross-compiler and appropriately configure the cmake toolchain file.

### Debian

Install MinGW toolchain package:

```bash
sudo apt install mingw-w64
```

If the compilation described further down fails complaining about undefined objects like `std::mutex` or `std::conditional_variable`,
switch to posix threading model:

```bash
sudo update-alternatives --config i686-w64-mingw32-gcc
<choose i686-w64-mingw32-gcc-posix from the list>

sudo update-alternatives --config i686-w64-mingw32-g++
<choose i686-w64-mingw32-g++-posix from the list>

sudo update-alternatives --config x86_64-w64-mingw32-gcc
<choose x86_64-w64-mingw32-gcc-posix from the list>

sudo update-alternatives --config x86_64-w64-mingw32-g++
<choose x86_64-w64-mingw32-g++-posix from the list>
```

### Arch Linux

Install mingw toolchain package:

```bash
sudo pacman -S mingw-w64
```

### Compilation

Configure cross-compilation cmake toolchain `Sources/cmake/TC-mingw.cmake` file by setting `CMAKE_C_COMPILER`, `CMAKE_CXX_COMPILER` and `CMAKE_FIND_ROOT_PATH`:

```cmake
# which compilers to use for C and C++
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
```

You can find more detailed explanation about the cmake toolchain file [here](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Cross%20Compiling%20With%20CMake.html).

Then configure and compile the project using:

```bash
cd Source/build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/TC-mingw.cmake ..
make
```

## Usage instruction

```bash
./emu [bios.bin] [game.bin]
```

TODO: images, save state, controller config, debug options

## Author information

 * Name: Filip Stupka 
 * Email: xstupk05@fit.vutbr.cz
 * Date: 2023/2024

