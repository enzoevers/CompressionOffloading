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
$ cd <root of this repositoory>
$ git submodule update --init
$ python ./Scripts/BuildAndInstallExternalLibs.py
$ python ./Scripts/BuildAndInstallCoDeLib.py
$ python ./Scripts/BuildBenchmark.py
$ python ./Scripts/RunBenchmark.py
```

Additionally, you should be able to set breakpoints in the source code and run the code and debugger via the Visual Studio Code UI.

# Zynq embedded linux

## Yocto development machine notes
> Note: This requires a Linux machine.

I used a separate desktop which I can access using remote desktop. That way I can let long yocto build run while I can just turn off my (Windows) laptop.
Tips for remote desktop usage:
- Enable autologin.
- Create a separate passwork passkey entry in ubuntu without a password, set it to default, update the password for the remote desktop in ubuntu settings, remove the default settings from the custom passkey.
    - If you don't do this, the remote desktop password will change on every reboot.
    - **Make sure** that the default is removed for the non-password passkey. Otherwise all passwords will be saved unencrypted. 
    - Source: https://askubuntu.com/a/1409857/1042231


## Yocto initial setup

> These steps are based on https://docs.yoctoproject.org/5.0.3/brief-yoctoprojectqs/index.html and changed where needed.

This project will use the `Scarthgap` release. See the [release list](https://wiki.yoctoproject.org/wiki/Releases).

> Note: The latest ubuntu versien that is [officially supported](https://docs.yoctoproject.org/5.0.3/ref-manual/system-requirements.html) by Scarthgap is 22.04 LTS. 

```sh
$ sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1
$ sudo locale-gen en_US.UTF-8
```

For the rest of this document we will work in a `yocto` directory. You can place it wherever you want.

```sh
$ cd <wherever you want>
$ mkdir yocto
$ cd yocto
```

```sh
$ git clone git://git.yoctoproject.org/poky
$ cd poky
$ git checkout tags/scarthgap-5.0.3 -b zybo-scarthgap-5.0.3
```

```sh
$ source oe-init-build-env
# You should be in the poky/build directory now 
```

When running the `bitbake` command below on an older system (I was running it on a leftover PC with an Intel i5-3470), you may encounter an error where the terminal simply closes. This is most likely due to resource exhoustion. 

To prevent this, in `Build/conf/local.conf`, add the following lines with appropriate values for your system. Note that you can also simply run the `bitbake` command without these changes. Then if the terminal would close due to recource exhoustion, add these lines and start/resume the build again.

```text
BB_NUMBER_THREADS = "2"
PARALLEL_MAKE = "-j 2"
PARALLEL_MAKEINST = "-j 2"
```

```sh
$ bitbake core-image-minimal
```

Now you should be able to run the qemu emulator

```sh
$ runqemu qemux86-64
````

## Add zybo layer

> Based on https://github.com/Xilinx/meta-xilinx/blob/scarthgap/README.building.md

```sh
$ cd <some path>/yocto
```

```sh
$ git clone -b scarthgap https://git.openembedded.org/meta-openembedded.git
$ git clone -b scarthgap https://git.yoctoproject.org/meta-virtualization
$ git clone -b scarthgap https://github.com/Xilinx/meta-xilinx.git
$ git clone -b scarthgap https://github.com/Xilinx/meta-xilinx-tools.git
```

```sh
$ source poky/oe-init-build-env
# You should be in the yocto/build directory now 
```

```sh
$ bitbake-layers remove-layer meta-yocto-bsp
$ bitbake-layers add-layer \
                        ${PWD}/../meta-openembedded/meta-oe \
                        ${PWD}/../meta-openembedded/meta-python \
                        ${PWD}/../meta-openembedded/meta-networking \
                        ${PWD}/../meta-openembedded/meta-filesystems \
                        ${PWD}/../meta-virtualization \
                        ${PWD}/../meta-xilinx/meta-microblaze \
                        ${PWD}/../meta-xilinx/meta-xilinx-core \
                        ${PWD}/../meta-xilinx/meta-xilinx-standalone \
                        ${PWD}/../meta-xilinx/meta-xilinx-bsp \
                        ${PWD}/../meta-xilinx/meta-xilinx-vendor \
                        ${PWD}/../meta-xilinx/meta-xilinx-contrib \
                        ${PWD}/../meta-xilinx-tools 
```

## refs
- https://forum.digilent.com/topic/20398-zybo-z7-10-using-yocto-instead-of-petalinux/
    - https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842481/Build+kernel?_ga=2.206565715.1530280883.1727806363-618129529.1727806363
- https://docs.yoctoproject.org/5.0.3/brief-yoctoprojectqs/index.html
- https://www.youtube.com/watch?v=2-PwskQrZac&list=PLEBQazB0HUyTpoJoZecRK6PpDG31Y7RPB&index=2