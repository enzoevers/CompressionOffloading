# Formatting

## Formatter
Download `clang-format` as part of `LLVM`: https://releases.llvm.org/download.html. While installing LLVM make sure to check the box `Add LLVM to the system PATH for ...`. Make sure to install at least version `16` to have `InsertNewlineAtEOF` available.

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
- Install **CMake**: https://cmake.org/download/ (>= 3.28)

## Linux

- Install **C development packages**: `$ apt install build-essential` 
- Install **CMake**: https://cmake.org/download/ (>= 3.28)

# Verify
Now you should be able build and run:

> Note that you may have to close and re-open viusal studio to get the updated PATH.

For some reason it only seems to work in PowerShell, not in the command prompt. This should be fixed.

```cmd
$ cd <root of this repositoory>
$ git submodule update --init
$ python ./Scripts/BuildAndInstallExternalLibs.py
$ python ./Scripts/BuildAndInstallCoDeLib.py
$ python ./Scripts/BuildBenchmark.py
$ python ./Scripts/RunBenchmark.py
$ python ./Scripts/RunTest.py
$
$ python ./Scripts/BuildAndInstallExternalLibs.py --TargetPlatform zynq
$ python ./Scripts/BuildAndInstallCoDeLib.py --TargetPlatform zynq
$ python ./Scripts/BuildBenchmark.py --TargetPlatform zynq
```

Additionally, you should be able to set breakpoints in the source code and run the code and debugger via the Visual Studio Code UI.
