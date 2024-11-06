# Zynq embedded linux with Petalinux

This section is heavily based on the FPGA series from the [Phil's Lab](https://www.youtube.com/@PhilsLab) youtube channel.
- Especially these video:
    - Vivado setup and hardware export: [FPGA/SoC Board Bring-Up Tutorial (Zynq Part 1) - Phil's Lab #96](https://www.youtube.com/watch?v=B-CbDfrfJRk&list=PLXSyc11qLa1ZutrEG2XmyWrNz17SSQTdH&index=2)
    - Petalinux configuration and build: [Embedded Linux + FPGA/SoC (Zynq Part 5) - Phil's Lab #100](https://www.youtube.com/watch?v=OfozFBfvWeY&list=PLXSyc11qLa1ZutrEG2XmyWrNz17SSQTdH&index=5)
    - Petalinux SD card boot: [FPGA/SoC SD Card + PetaLinux (Zynq Part 6) - Phil's Lab #135](https://www.youtube.com/watch?v=XCCTX0fhcBs&list=PLXSyc11qLa1ZutrEG2XmyWrNz17SSQTdH&index=10)

The steps below are adapted for the specific vivado and petalinux version that I used. They are also tailored for the [(original) Zybo](https://digilent.com/reference/programmable-logic/zybo/start?srsltid=AfmBOop7Qam8vkcVaiuPNhz0zAWlaV2d1hUVQOqYETs5qriRFPGwHxVa) development board. 

## Software and hardware version used in these steps
- Windows 11 host: `10.0.22631 Build 22631`
- VMware® Workstation 17 Player - 17.5.2 build-23775571
- Ubuntu 22.04.5 LTS (Jammy Jellyfish)
    - Installed in VMware. 
- Linux 6.8.0-47-generic
- [petalinux-v2024.1-05202009-installer](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html)
    - Installed in the Ubuntu VM.
- [Vitis unified software platform 2024.1](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html). This includes Vivado.
    - Vivado version installed via this installer: `SW Build: 5076996`
    - Downloading the Vivado installer would be sufficient for these steps. However, vitis will be installed as well anyway, but in the 2024.1 this vitis launcher doesn't seem to work well yet. You would need to manually add the `--classic` flag when opening Vitis. Installing the Vitis unified software platform also installes Vivado and doesn't have this problem.
    - Note that you only have to select support for the `Zynq-7000` platform. Enabling only this saves quite some space.

## Steps

These steps have been tested on a fresh Ubuntu install in the VM. See the previous section for the specific versions.

<details>
  <summary>1.0 Installing system dependencies</summary>

### 1.0 Installing system dependencies

```sh
$ sudo apt install update
$ sudo apt install upgrade
```

```sh
$ sudo apt install gcc xterm autoconf libtool texinfo zlib1g-dev gcc-multilib build-essential zlib1g libncurses5 libncurses5-dev
```

</details>

<details>
  <summary>2.0 Download and install Vivado (Vitis unified software platform) on the host machine</summary>

### 2.0 Download and install Vivado (Vitis unified software platform) on the host machine

> Download link: https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html

On the first page select Vitis.

<img src=./images/VitisUFI_SelectVitis.png>

On the second page only select the Zynq-7000

<img src=./images/VitisUFI_SupportedTarget.png>

</details>

<details>
  <summary>3.0 Get the Zybo board files</summary>

### 3.0 Get the Zybo board files

> Based on https://digilent.com/reference/programmable-logic/guides/installing-vivado-and-vitis#install_digilent_s_board_files

Get the digilentic board files from https://github.com/Digilent/vivado-boards. 

>If this repository is for some reason not avaialble anymore you can download the zybo board files from this repostory under [./ZyboVivadoBoardFiles/](./ZyboVivadoBoardFiles/).

Copy the board file into `<Xilinx install dir>/Vivado/data/boards/board_files`. If the `board_files` doesn't exist, create it.

</details>

<details>
  <summary>4.0 Create and export a based bitstream from Vivado</summary>

### 4.0 Create and export a based bitstream from Vivado

Create a new project in Vivado and select RTL Project without sources.

<img src=./images/VivadoProject_RTLProject.png>

Select the Zybo board.

<img src=./images/VivadoProject_ZyboBoard.png>

Now finish and Vivado should open.

Create a new block design.

<img src=./images/VivadoProject_CreateBlockDesign.png>

Add both the `ZYNQ7 Processing System` and `Processor System REset` IPs to the block design and run both **block** and **connection** automation. During connection automation select all. Note that the connection between "FCLK_CLK0" and "M_AXI_GP0_ACLK" must be made manually.

<img src=./images/VivadoProject_AddIP.png>

The result would look like this:

<img src=./images/VivadoProject_AutoConfigure.png>

Create a HDL wrapper for the block design by right blocking on the block design source and selecting "Create HDL wrapper". Everything can stay default.

<img src=./images/VivadoProject_AutoConfigure.png>

Now generate a bitstream. Leave everything default.

<img src=./images/VivadoProject_GenerateBitstream.png>

Lastly export the hardware and make sure to include the bitstream. Keep a note on where the `<name>.xsa` file will be exported for the next step.

<img src=./images/VivadoProject_ExportHardware.png>

<img src=./images/VivadoProject_IncludeBitstream.png>

</details>

<details>
  <summary>5.0 Copy the exported hardware to the VM</summary>

### 5.0 Copy the exported hardware to the VM

Go to the exported `.xsa` file and copy it to the Ubuntu VM. In these steps it will be placed in `~/vivado_design`.

```sh
$ mkdir ~/vivado_design
```

</details>

<details>
  <summary>6.0 Download and install the petalinux tools in the Ubuntu VM</summary>

### 6.0 Download and install the petalinux tools in the Ubuntu VM

> Download link: https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html

```sh
$ cd <download placee of the installer>
$ sudo cmod a+x petalinux-v2024.1-05202009-installer.run
```

Only the zynq support is required. Therefore use `--platform "arm"` when installing. Information about the other parameters can be found here: (https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/Installing-the-PetaLinux-Tool).

```sh
// Note: No sudo
$ ./petalinux-v2024.1-<your versoin>-installer.run -D --dir ~/petalinux/ --platform "arm"
```

</details>

<details>
  <summary>7.0 Create a petalinux project</summary>

### 7.0 Create a petalinux project

The petalinux tools must first be sourced:

```sh
$ cd ~/petalinux/
$ source ./settings.sh
```

A new project is made with the custom name `BasicLinuxZybo`.

```sh
$ petalinux-create project --template zynq --name BasicLinuxZybo
```

</details>

<details>
  <summary>8.0 Configure and build the petalinux project</summary>

First go to the just created project.

```sh
$ cd BasicLinuxZybo
```

### 8.0 Configure and build the petalinux project

For more details see [FPGA/SoC SD Card + PetaLinux (Zynq Part 6) - Phil's Lab #135](https://www.youtube.com/watch?v=XCCTX0fhcBs&list=PLXSyc11qLa1ZutrEG2XmyWrNz17SSQTdH&index=10) from [Phil's Lab](https://www.youtube.com/@PhilsLab)

#### 8.1 Hardware

From within the `BasicLinuxZybo` directory, initialize the project with the exported hardware from Vivado. A menu will open after some time. No need to change anything in the opened config menu. It can be closed.

```sh
$ petalinux-config --get-hw-description=/home/${USER}/vivado_design/design_1_wrapper.xsa
```

> For more information see https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/petalinux-config

Now additional configuration must be done.

#### 8.2 Non-volatile rootfs

```sh
$ petalinux-config
```

In the openened menu:
- Go to `Image Packaging Configuration > Root file system type`
- Select `EXT4 (SD/eMMC/SATA/USB)`

If you didn't setup tftpboot on Ubuntu then you can also disable `Copy final images to tftpboot` in the `Image Packaging Configuration` menu.

Exit and save.

#### 8.3 Kernel

```sh
$ petalinux-config -c kernel
```

- Bitbake will now start. It takes some time.
- A new config windows opens. No changes are needed. It can be closed.

#### 8.4 u-boot

```sh
$ petalinux-config -c u-boot
```

- This will take some time again.
- A new config screen opens. Changes are needed.
    - In boot options enable boot from SD.
    - Enable `Boot options > Boot media > Support for booting from SD/EMMC`.
    - Exit and save

#### 8.5 rootfs

```sh
$ petalinux-config -c rootfs
```

- In the config enable `user packages > peekpoke`.
- Exit and save

#### 8.6 Build the project

```sh
$ petalinux-build
```

#### 8.7 Generate BOOT.BIN

The generated files from the petalinux build can be found in the location `~/petalinux/BasicLinuxZybo/images/linux`.

If you don't see a `BOOT.BIN` file, generate it with this command:

```sh
$ cd ~/petalinux/BasicLinuxZybo
```

```sh
$ petalinux-package boot --force --fsbl --fpga --u-boot
```

To see what these flags do excactly, see the petalinux documentation: https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/petalinux-package

</details>

<details>
  <summary>9.0 Options 1: Creating a bootable SD card manually</summary>

### 9.0 Options 1: Creating a bootable SD card manually

#### 9.1 SD card preperation

<img src=./images/ZynqBootableSDFormat.png width=50%>

The partitions can be created using `fdisk` To see how to do this either follow for example the video from [Phil's Lab](https://youtu.be/XCCTX0fhcBs?si=5JDLc_1IcE80-Eto&t=775) or read the [Xilinx documentation](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842385/How+to+format+SD+card+for+SD+boot).

Just make sure that the first (boot) partition is marked as bootable.

The name of your disk can be found by running, for example, `$ sudo lsblk`. Run it before and after inserting your SD card and see which name was added. In my case the SD card name is `sdb`.

```sh
$ sudo fdisk /dev/<your SD card>
```

These partitions still have to be formatted using the commands below.

```sh
$ sudo mkfs.vfat -F 32 -n boot /dev/sdb1
$ sudo mkfs.ext4 -L root /dev/sdb2
```

#### 9.2 Moving files to the SD card

All generated files from `petalinux-build` are placed in the following path.

```sh
$ cd ~/petalinux/BasicLinuxZybo/images/linux
```

##### Boot partition

Copy the following files to the **boot** partition on the SD:

- `boot.scr`
- `BOOT.BIN`
- `image.ub`

##### Rootfs partition

Now extract the root file system to the **rootfs** partition.

> [!IMPORTANT]
> Make sure to double check the name of the SD card (partition)

```sh
$ sudo dd if=rootfs.ext4 of=/dev/<second partition of your SD> status=progress
```

</details>

<details>
  <summary>9.0 Options 2: Creating a bootable SD card with wic image</summary>

### 9.0 Options 2: Creating a bootable SD card with wic image

By using wic, prepering the SD card and moving files is all handled automatically.

#### 9.1 Create the wic image

```sh
$ cd ~/petalinux/BasicLinuxZybo
```

The command below uses `~/petalinux/BasicLinuxZybo/build/rootfs.wks` for creating the wic image. By default (in the version used in these steps) this will create a `2GB` boot partition and a `4G` root partition. So make sure that your SD card is large enough. If you want to change this, see the [petalinux-package wic Command Options](https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/petalinux-package-wic-Command-Options) documentation.

```sh
$ petalinux-package wic
```

The console output will show where the wic image is created.

<details>
  <summary>(Optional) 9.1.1 Inspect the wic image</summary>

#### (Optional) 9.1.1 Inspect the wic image

```sh
# Make wic easily available
$ export PATH=${PATH}:${PWD}/components/yocto/sysroots/x86_64-petalinux-linux/usr/bin/

# To see the first partition
$ sudo apt install mtools
```

```sh
$ cd ~/petalinux/BasicLinuxZybo/images/linux
```

```sh
$ wic ls petalinux-sdimage.wic
$ wic ls petalinux-sdimage.wic:1
$ wic ls petalinux-sdimage.wic:2
```

</details>

#### 9.2 Write the wic image the SD card

In my case the `.wic` image was placed in the path below.

```sh
$ cd ~/petalinux/BasicLinuxZybo/images/linux
```

<details>
  <summary>9.2.1 Options 1: Flash using dd command</summary>

##### 9.2.1 Options 1: Flash using `dd` command

You may want to remove any partitions from the SD before doing the next steps. This can, for example, be done with the built-in `Disks` application in Ubuntu.

The name of your disk can be found by running, for example, `$ sudo lsblk`. Run it before and after inserting your SD card and see which name was added. In my case the SD card name is `sdb`.

> [!IMPORTANT]
> Make sure to double check the name of the SD card

```sh
$ sudo dd if=petalinux-sdimage.wic of=/dev/<your SD card> status=progress
```

</details>

<details>
  <summary> 9.2.1 Options 2: Flash using BalenaEtcher application</summary>

##### 9.2.1 Options 2: Flash using `BalenaEtcher` application

When I tried using the `dd` command, for some reason the SD card would be disconnected from my VM halway through the `dd` process. You can also use [BalenaEtcher](https://etcher.balena.io/) on Windows to flash the SD card with the `petalinux-sdimage.wic` image.

</details>

</details>

<details>
  <summary>10.0 First boot</summary>

### 10.0 First boot

#### 10.1 Set correct jumpers on the Zybo board

There are two jumpers to set. The power jumper and the boot jumper.

For all the information see https://digilent.com/reference/programmable-logic/zybo/reference-manual#power_supplies. The PDF version is also in this repository at [./pdf/ZyboReferenceManual.pdf](./pdf/ZyboReferenceManual.pdf).

##### Power selection jumper

This jumper is next to the ON/OFF switch. The silkscreen next to it shows the different configurations.

USB power is sufficient for this basic project.

##### Boot selection jumper

This jumper is below the VGA connector. Also here the silkscreen shows the different configurations.

For this project the SD selection is required.

#### 10.2 Boot

Now insert the SD card at the bottom. Connect a USB cable to the PROG/UART USB port, turn on the Zybo, and see as which (COM) port it shows up on the PC.

Open a serial reader, for example putty, and connect to the Zybo.
- Baud: 115200
- Data bits: 8
- Stop bits: 1
- No parity
- No flow control

Press the `PS-SRST` (BTN7) on the Zybo to see the whole boot process from the start in the console.

When prompted to log in use the following credentials:

- user: petalinux
- Required to create new password

</details>

## Refs
- https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM
- https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/Navigating-Content-by-Design-Process
- https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html
- https://github.com/Digilent/Petalinux-Zybo/tree/v2017.4-1?_ga=2.105949445.786026895.1729948895-618129529.1727806363
- https://forum.digilent.com/topic/25406-old-zybo/
- https://adaptivesupport.amd.com/s/question/0D52E00006hpY4LSAU/adding-xsa-to-yocto-layer?language=en_US
- https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide
- https://www.youtube.com/watch?v=OfozFBfvWeY&list=PLXSyc11qLa1ZutrEG2XmyWrNz17SSQTdH&index=5
- https://github.com/Digilent/Petalinux-Zybo?_ga=2.105949445.786026895.1729948895-618129529.1727806363
- https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842385/How+to+format+SD+card+for+SD+boot