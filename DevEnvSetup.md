# Formatting

## Formatter
Download `clang-format` as part of `LLVM`: https://releases.llvm.org/download.html. While installing LLVM make sure to check the box `Add LLVM to the system PATH for ...`.

> Note that you may have to close and re-open viusal studio to get the updated PATH.

## Git hooks
Run:

```cmd
git config --local core.hooksPath ./GitHooks
```

# IDE related setup

## Visual studio (any OS)
Install the recommended packages for this reposity (see `./vscode/extensions.json`)

# OS related setup

## Windows

- Setup **MinGW** by following the steps from https://code.visualstudio.com/docs/cpp/config-mingw#_prerequisites or the steps below.
    1. Install MSYS2: https://www.msys2.org/
    2. Open MSYS2 and run `pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain`
    3. Add the MinGW-w64 bin folder (most likely `C:\msys64\ucrt64\bin`) to the Windows PATH
- Install **CMake**: https://cmake.org/download/

## Linux

- Install **C development packages**: `$ apt install build-essential` 
- Install **CMake**: https://cmake.org/download/

# Verify
Now you should be able build and run:

> Note that you may have to close and re-open viusal studio to get the updated PATH.

```cmd
C:<path to root of this project> python .\Scripts\BuildAll.py
C:<path to root of this project> Build\Benchmark.exe
```

Additionally, you should be able to set breakpoints in the source code and run the code and debugger via the Visual Studio Code UI.