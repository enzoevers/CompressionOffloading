# Cross compiling for (Zybo) petalinux

This document assumes that the petalinux setup guide ([ZyboPetalinuxGuide.md](./ZyboPetalinuxGuide.md)) has been followed.

## In the Ubuntu VM

### Petalinux SDK

```
$ petalinux-build --sdk
```

Halfway through while running this in the VM, I got the error that `https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.xz` could not be reached/downloaded. When I changed the network settings from NAT to Bridged in VMware it could be found.

```sh
$ petalinux-package sysroot
```

Only a couple directories are needed (https://doayee.co.uk/petalinux-on-windows-via-wsl-and-git/):

- `~/petalinux/BasicLinuxZybo/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/usr`
- `~/petalinux/BasicLinuxZybo/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/lib`

Copy [`ExportPetalinuxSdk.sh`](./ExportPetalinuxSdk.sh) into the VM.

This script must be executed on each change of your Petalinux build. 

```sh
$ sudo ./ExportPetalinuxSdk.sh ~/petalinux/BasicLinuxZybo/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/ ./ZyboSdkSysroot/
$ sudo chown -R $USER:$USER ./ZyboSdkSysroot
$ cd ./ZyboSdkSysroot; zip -r ~/ZyboSdkSysroot.zip *; cd ../
```

> An [`ZyboSdkSysroot.zip`](./ZyboSdkSysroot.zip), based on [ZyboPetalinuxGuide.md](./ZyboPetalinuxGuide.md), is present in this repostory

The cross compiler is required as well. Copy [`ExportPetalinuxCrossCompiler.sh`](./ExportPetalinuxCrossCompiler.sh) into the VM. Execute it as follows:

```sh
$ sudo ./ExportPetalinuxCrossCompiler.sh ~/petalinux/BasicLinuxZybo/images/linux/sdk/sysroots/x86_64-petalinux-linux/ ./compiler/
$ sudo chown -R $USER:$USER ./compiler
$ cd ./compiler; zip -r ~/compiler.zip *; cd ../
```

### Cross compile

```cmake
cmake_minimum_required(VERSION 3.05)

SET(CMAKE_C_FLAGS "-g -Wall -Wextra -O0 -g3")

project(ZynqHelloWorld LANGUAGES C VERSION 1.0)

add_executable(${CMAKE_PROJECT_NAME} main.c)
```

```c
#include <stdio.h>

int main()
{
	printf("Hi from Zybo\n");
	return 0;
}
```

```sh
$ source ~/petalinux/BasicLinuxZybo/images/linux/sdk/environment-setup-cortexa9t2hf-neon-xilinx-linux-gnueabi
```

```sh
$ cmake -S . -B build \
    -DCMAKE_SYSROOT="~/ZyboSdkSysroot"
$ cmake --build build
```

### Test on the Zybo

Plug an ethernet cable in the Zybo, boot the Zybo, open a serial terminal to connect it to, and check its IP address (with `ifconfig`).

In the VM

```sh
$ scp build/ZynqHelloWorld petalinux@<ip address of Zybo>:~/
```

On the Zybo

```sh
$ cd ~/
$ ./ZynqHelloWorld
# This should print "Hi from Zybo"
```

## In Windows

### Install Vitis unified platform

It is assumed that this is already done in [ZyboPetalinuxGuide.md](./ZyboPetalinuxGuide.md).

Copy the `ZyboSdkSysroot` folder, which you created as part of the [SDK step](#petalinux-sdk) in this document, somewhere on your windows host. The rest of this document assumes that it has been extracted to `C:/ZyboSdkSysroot/`

### Cross compile

Create a `main.c` file with the following content:

```c
#include <stdio.h>

int main() {
    printf("Hi from Zybo\n");
    return 0;
}
```

```ps
PS > C:/Xilinx/Vitis/2024.1/settings64.bat
```

The command below is derived from the `compile_commands.json` that is genereated when creating a basic linux project based on the exported `.xsa` in Vits.

#### Without CMake

```ps
PS > C:/Xilinx/Vitis/2024.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-gcc.exe `
        --sysroot=C:/Xilinx/Vitis/2024.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/cortexa9t2hf-neon-xilinx-linux-gnueabi `
        -g `
        -Wall `
        -Wextra `
        -O0 `
        -g3 `
        main.c
```

```ps
PS > C:/Xilinx/Vitis/2024.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-gcc.exe `
        --sysroot=C:/ZyboSdkSysroot/ `
        -g `
        -Wall `
        -Wextra `
        -O0 `
        -g3 `
        main.c
```

This generated a `a.out` file.

### Test on the Zybo

```ps
PS > scp a.out petalinux@192.168.1.87:~/
```

On the Zybo

```sh
$ cd ~/
$ chmod a+x a.out
$ ./a.out
# This should print "Hi from Zybo"
```

#### With CMake

Create a `CMakeLists.txt` file with the following content

```cmake
cmake_minimum_required(VERSION 3.05)

SET(CMAKE_C_FLAGS "-g -Wall -Wextra -O0 -g3")

project(ZynqHelloWorld LANGUAGES C VERSION 1.0)

add_executable(${CMAKE_PROJECT_NAME} main.c)
```

From the "<some path>\VitisWorkspace\logs\vitis.log" file it could be seen there Vitis provides a simple `zynq.toolchain` file to be used in cmake. This does require to add a path our environment path variable.

Install cmake: https://cmake.org/download/

> Note: for the application in this repository the compiler `"C:\Xilinx\Vitis\2024.1\gnu\aarch32\nt\gcc-arm-linux-gnueabi\x86_64-petalinux-mingw32\usr\bin\arm-xilinx-linux-gnueabi\arm-xilinx-linux-gnueabi-gcc.exe"` is used instead. For this compiler to work it is required to add the following flags when setting `CMAKE_C_COMPILER`: `set(CMAKE_C_COMPILER "C:/Xilinx/Vitis/2024.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/x86_64-petalinux-mingw32/usr/bin/arm-xilinx-linux-gnueabi/arm-xilinx-linux-gnueabi-gcc.exe -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 -fstack-protector-strong -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security") But for the example in this document using this compiler is fine.

```ps
PS > $env:path = "${env:path};C:/Xilinx/Vitis/2024.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin"
```

```ps
PS > cmake -S . -B build `
        -G "MinGW Makefiles" `
        -DCMAKE_TOOLCHAIN_FILE="C:/Xilinx/Vitis/2024.1/vitisng-server/scripts/cmake/zynq.toolchain" `
        -DCMAKE_SYSROOT="C:/ZyboSdkSysroot/"
PS > cmake --build build
```

### Test on the Zybo

```ps
PS > scp .\build\ZynqHelloWorld petalinux@<ip address of Zybo>:~/
```

On the Zybo

```sh
$ cd ~/
$ ./ZynqHelloWorld
# This should print "Hi from Zybo"
```

### Talking with custom FPGA components

## Refs

- https://docs.amd.com/r/en-US/ug1400-vitis-embedded/Setting-Up-the-Environment-to-Run-the-Vitis-Software-Platform
- https://docs.amd.com/r/2022.2-English/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-Arm
- https://adaptivesupport.amd.com/s/question/0D54U00006qVENrSAO/difference-vitis-and-sdk-crosscompilation-toolchains?language=en_US
- https://adaptivesupport.amd.com/s/question/0D54U00007jZLyoSAG/cmake-cross-compilation-with-vitis20222-on-petalinux?language=en_US
- https://doayee.co.uk/petalinux-on-windows-via-wsl-and-git/
