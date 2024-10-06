# Zynq embedded linux with Yocto

> [!IMPORTANT]
> This document is only a collection of steps that have been tried. No successful linux image has been created using these steps. This file will stay here to be updated when fix would have been found.

## Remote development machine notes

I used a separate ubuntu desktop which I can access using remote desktop. That way I can let long yocto build run while I can just turn off my (Windows) laptop.
Tips for remote desktop usage:
- Enable autologin.
- Create a separate passwork passkey entry in ubuntu without a password, set it to default, update the password for the remote desktop in ubuntu settings, remove the default settings from the custom passkey.
    - If you don't do this, the remote desktop password will change on every reboot.
    - **Make sure** that the default is removed for the non-password passkey. Otherwise all passwords will be saved unencrypted. 
    - Source: https://askubuntu.com/a/1409857/1042231

# Plain yocto

## Checking that the default yocto build works

Setup for this section:
- Using `Scarthgap 5.0.3`
    - See the [release list](https://wiki.yoctoproject.org/wiki/Releases)
- The steps are based on https://docs.yoctoproject.org/5.0.3/brief-yoctoprojectqs/index.html and updated where required
- Ubuntu 22.04 desktop
    - The latest ubuntu versien that is [officially supported](https://docs.yoctoproject.org/5.0.3/ref-manual/system-requirements.html) by Scarthgap is 22.04 LTS
    - The steps are based on a clean install

> In principle this part can be skipped. However, this part makes sure that at least the most basic build of yocto can be succesfull. When something goes wrong in the next part which is zybo specific, then at least it is clear that the problem is with that part and not with the main build system.

First install some requied packages and set the localization files.

```sh
$ sudo apt install update
$ sudo apt install upgrade
```

```sh
$ sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1
```

```sh
$ sudo locale-gen en_US.UTF-8
```

For the rest of this document we will work in a `yocto` directory. You can place it wherever you want.

```sh
$ cd <wherever you want>
$ mkdir yocto
$ cd yocto
```

Then get the poky base project for yocto. As mentioned above, `Scarthgap 5.0.3` will be used.

> The latest commit used in this document for `tags/scarthgap-5.0.3` was [`0b37512fb4b231cc106768e2a7328431009b3b70`](https://git.yoctoproject.org/poky/tag/?h=scarthgap-5.0.3).

```sh
$ git clone git://git.yoctoproject.org/poky
$ cd poky
$ git checkout tags/scarthgap-5.0.3 -b zybo-scarthgap-5.0.3
```

Now generate the build directory and make the required programs available.

```sh
$ source oe-init-build-env
# You should be in the poky/build directory now 
```

The current target machine is `qemux86-64`. The next command will create a minial linux image for the qemu target.

<a name="resource-exhaustion-tip"></a><blockquote>

When running the `bitbake` command below on an older system (I was running it on a leftover PC with an Intel i5-3470), you may encounter an error where the terminal simply closes after some time. This is most likely due to resource exhaustion. 

To prevent this, in `Build/conf/local.conf`, add the following lines with appropriate values for your system. Note that you can also run the `bitbake` command without these changes. Then if the terminal would close due to recource exhaustion, add these lines and start/resume the build again with the same `bitbake` command.

To see the current value run `$ bitbake-getvar <variable>`

```text
# Build/conf/local.conf

BB_NUMBER_THREADS = "2"
PARALLEL_MAKE = "-j 2"
PARALLEL_MAKEINST = "-j 2"
```

</blockquote>

```sh
$ bitbake core-image-minimal
```

Now you should be able to run the qemu emulator (from the build directory)

```sh
$ runqemu qemux86-64
```

## Add zybo layer

Setup for this section:
- Follow the [previous section](#checking-that-the-default-yocto-build-works) up until checking out the `scarthgap-5.0.3` poky tag.
- The steps are based on https://github.com/Xilinx/meta-xilinx/blob/scarthgap/README.building.md and updated where required

First go to your `yocto` directory.

### Get the required layers

```sh
$ cd <some path>/yocto
```

<blockquote>

The following commits were used during this write-up:
- meta-openembedded: `scarthgap` | [72018ca1b1a471226917e8246e8bbf9a374ccf97](https://git.openembedded.org/meta-openembedded/commit/?h=scarthgap&id=72018ca1b1a471226917e8246e8bbf9a374ccf97)
- meta-virtualization: `scarthgap` | [6f3c1d8f90947408a6587be222fec575a1ca5195](https://git.yoctoproject.org/meta-virtualization/commit/?h=scarthgap&id=6f3c1d8f90947408a6587be222fec575a1ca5195)
- meta-xilinx: `scarthgap` | [03d3b2ce359a1c2959f04588cf2c4a1b0dcb4de8](https://github.com/Xilinx/meta-xilinx/commit/03d3b2ce359a1c2959f04588cf2c4a1b0dcb4de8)
- meta-xilinx-tools: `scarthgap` | [338f31d1a7ee07e6408925b2101ff8d364792367](https://github.com/Xilinx/meta-xilinx-tools/commit/338f31d1a7ee07e6408925b2101ff8d364792367)

 Most likely newer commits will work 'just fine'. But these commits worked in this write-up.

</blockquote>

```sh
$ git clone -b scarthgap https://git.openembedded.org/meta-openembedded.git
$ git clone -b scarthgap https://git.yoctoproject.org/meta-virtualization
$ git clone -b scarthgap https://github.com/Xilinx/meta-xilinx.git
$ git clone -b scarthgap https://github.com/Xilinx/meta-xilinx-tools.git
```

### Source the environment

Now create the build folder and source the required programs.

```sh
$ source poky/oe-init-build-env
# You should be in the yocto/build directory now 
```

### Add layers to the bitbake configuration

<blockquote>

Note that the order of the layers in the `add-layer` command matters. This is because the different layers have dependencies.

For example. If you look in `meta-openembedded/meta-filesystems/conf/layer.conf`, you will see the lines below. This shows that it requires the `networking-layer` layer (the `LAYERDEPENDS_filesystems-layer` variable). This layer happens to be in  `meta-openembedded/meta-networking`. The `networking-layer` can be found as the `BBFILE_COLLECTIONS` variable in `meta-openembedded/meta-networking/conf/layer.conf`.

```bitbake
# meta-openembedded/meta-filesystems/conf/layer.conf

BBFILE_COLLECTIONS += "filesystems-layer"

...

LAYERDEPENDS_filesystems-layer = "core openembedded-layer networking-layer"
```

</blockquote>

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

alt while testing

```sh
$ bitbake-layers add-layer \
                        ${PWD}/../meta-openembedded/meta-oe \
                        ${PWD}/../meta-xilinx/meta-xilinx-core \
                        ${PWD}/../meta-xilinx/meta-xilinx-standalone \
                        ${PWD}/../meta-xilinx/meta-xilinx-vendor \
                        ${PWD}/../meta-xilinx-tools 
```

### Update `build/conf/local.conf`

Change the target machine from qemu to the zybo. In `meta-xilinx/meta-xilinx-vendor/conf/machine/zybo-zynq7.conf` the machine name for the zybo board can be found as `zybo-zynq7`.

Change this in `yocto/build/conf/local.conf`.

```
# build/conf/local.conf

#MACHINE ??= "qemux86-64"
MACHINE = "zybo-zynq7"
```

In the same file also add the following lines to specify the type of image to create to flash to the SD card later.

``` 
# build/conf/local.conf

IMAGE_FSTYPES += "wic"
WKS_FILES = "xilinx-default-sd.wks"
```

To remove a warning from the `meta-virtualization` layer add the following in `yocto/build/conf/local.conf`.

```
# build/conf/local.conf

DISTRO_FEATURES:append = " virtualization"
```

Optional: For potentially quicker builds, uncomment the lines below in `yocto/build/conf/local.conf`.

<blockquote>

Using these mirrors requires python websockets

```sh
$ pip install websockets
```

</blockquote>

```
# build/conf/local.conf

BB_HASHSERVE_UPSTREAM = "wss://hashserv.yoctoproject.org/ws"
SSTATE_MIRRORS ?= "file://.* http://cdn.jsdelivr.net/yocto/sstate/all/PATH;downloadfilename=PATH"

...

BB_HASHSERVE = "auto"
BB_SIGNATURE_HANDLER = "OEEquivHash"
```

### QEMU setup

```sh
$ bitbake qemu-helper-native
```

Setup tap interfaces in order to use the output of `core-image-minimal` in qemu.

```sh
$ sudo ../poky/scripts/runqemu-gen-tapdevs $(id -u $USER) $(id -g $USER) 4 tmp/sysroots-components/x86_64/qemu-helper-native/usr/bin
```

### Build the main image

When I first ran the final `bitbake` command I got an error that it failed to compile `linux-xlnx-6.6.10` due to not being able to find `zynq-zybo.dtb`. To fix this, update the following in `meta-xilinx/meta-xilinx-vendor/conf/machine/zybo-zynq7.conf` (add a `xilinx/` prefix):

<blockquote>

It's looking in the `build/tmp/work-shared/zybo-zynq7/kernel-source/arch/arm/boot/dts` directory.

I have created a PR for this: https://github.com/Xilinx/meta-xilinx/pull/64

</blockquote>

```diff
- KERNEL_DEVICETREE = "zynq-zybo.dtb"
+ KERNEL_DEVICETREE = "xilinx/zynq-zybo.dtb"
```

Before running the final command, please read the [tip above](#resource-exhaustion-tip) about resource exhaustion.
Now you are ready for building the main minimal linux image for the zybo target. Note that this steps can take several hours depending on your machine.

```sh
$ bitbake core-image-minimal
```

<blockquote>

Another time, when building in a VM, I got the error when building openssl and/or libgcrypt `gmp-6.3.0-r0 do_package: Error executing a python function in exec_func_python() autogenerated`

</blockquote>

#### Verify with QEMU

When this is finished you can use qemu to verify that the build was successful.

```sh
$ runqemu nographic
```

Login with `root`. No password is required.

> Note: To terminate qemu, usually the command is `ctrl-a` `x`

https://docs.yoctoproject.org/5.0.3/singleindex.html#creating-partitioned-images-using-wic

## Create a bootable SD card

### Using `wic`

First have a look into what is in the wic file.

```sh
$ sudo apt install mtools # to see the first partition
$ wic ls build/tmp/deploy/images/zybo-zynq7/core-image-minimal-zybo-zynq7.rootfs-<timestamp>.wic
$ wic ls build/tmp/deploy/images/zybo-zynq7/core-image-minimal-zybo-zynq7.rootfs-<timestamp>.wic:1
$ wic ls build/tmp/deploy/images/zybo-zynq7/core-image-minimal-zybo-zynq7.rootfs-<timestamp>.wic:2
```

> The same file (different name `build/xilinx-default-sd-<timestamp>-sda.direct`) can be generated with `$ wic create xilinx-default-sd -e core-image-minimal`. This uses the `./meta-xilinx/meta-xilinx-core/wic/xilinx-default-sd.wks` file to specify the partitions.

Unfortunaltely this didn't work for me. I tried several things to make it work, and also tried to manually copy files to the SD card, but it didn't help. I will leave this here, however. When I later find a fix that I can update it here.

## Refs
- https://gist.github.com/Scott31393/e7e3c952cee327fe0496ae09e3e839cd
- https://forum.digilent.com/topic/20398-zybo-z7-10-using-yocto-instead-of-petalinux/
    - https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842481/Build+kernel?_ga=2.206565715.1530280883.1727806363-618129529.1727806363
- https://docs.yoctoproject.org/5.0.3/brief-yoctoprojectqs/index.html
- https://www.youtube.com/watch?v=2-PwskQrZac&list=PLEBQazB0HUyTpoJoZecRK6PpDG31Y7RPB&index=2
- https://github.com/Xilinx/yocto-manifests
- https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/2824503297/Building+Linux+Images+Using+Yocto
- https://stackoverflow.com/questions/48674104/clang-error-while-loading-shared-libraries-libtinfo-so-5-cannot-open-shared-o
- https://www.youtube.com/watch?v=FMCfn0zwhaQ
- https://digilent.com/reference/learn/programmable-logic/tutorials/zybo-zybot-guide/start
- https://forum.digilent.com/topic/25406-old-zybo/
- https://miscircuitos.com/tutorial-zybo-linux-i-how-to-load-linux-debian-in-zybo-zynq-with-a-sd-card/?_ga=2.105949445.786026895.1729948895-618129529.1727806363
