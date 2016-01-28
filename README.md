Fernly - Fernvale Reversing OS
========================================

Fernly is a simple operating system designed for use in the reverse engineering
of the Fernvale CPU.  It will likely be disposed of when the system has been
understood well enough to implement a full operating system.


Setting up cross compilation
----------------------------
### Linux

    git clone https://github.com/robertfoss/setup_codesourcery.git
    sudo setup_codesourcery/setup.sh
    /usr/local/bin/codesourcery-arm-2014.05.sh


Building Fernly
---------------

To compile, simply run "make".  If you're cross-compiling, set CROSS_COMPILE to
the prefix of your cross compiler.  This is very similar to how to compile for Linux.

For example:

    make CROSS_COMPILE=arm-none-linux-gnueabi-


Running Fernly
--------------

To run, connect the target device and run the following command:

    ./build/fernly-usb-loader -s /dev/fernvale ./build/usb-loader.bin ./build/firmware.bin

This will open up /dev/fernvale, load usb-loader.bin as a stage 1 bootloader,
and then load (and jump to) firmware.bin as stage 2.  Optionally, you can add
a stage 3 file by specifying it as an additional argument.

Many 3rd-party devices enter bootloader mode only for a short window (~1s)
after being connected to USB. A device almost certainly should be "off". Some
devices require that battery is removed, while some - don't. To accommodate
such cases, there's -w (wait) option. Run fernly-usb-loader, and only
then connect a device to USB. This will allow to try various combinations
mentioned above with greater comfort (you need to disconnect and poweroff
device after each try, and restart fernly-usb-loader).

    ./build/fernly-usb-loader -w -s /dev/ttyUSB0 ./build/usb-loader.bin ./build/firmware.bin

Linux Notes
-----------

Since Fernvale is based on a Mediatek chip, ModemManager will, by default,
try to treat it as a modem and make it available for network connections.
This is undesirable.

To work around this problem, create a udev rule under /etc/udev/rules.d/
called 98-fernvale.rules with the following contents:

    SUBSYSTEM=="tty", ATTRS{idVendor}=="0e8d",\
        ATTRS{idProduct}=="0003",\
        MODE="0660", SYMLINK+="fernvale"

    ACTION=="add|change", SUBSYSTEM=="usb",\
        ENV{DEVTYPE}=="usb_device", ATTRS{idVendor}=="0e8d",\
        ATTRS{idProduct}=="0003",\
        ENV{ID_MM_DEVICE_IGNORE}="1"

OSX Notes
---------
The default OSX CDC matching seems to miss the Fernvale board. Use [fernvale-osx-codeless](https://github.com/jacobrosenthal/fernvale-osx-codeless) to get a com port.


SPI and Flashrom
----------------

Fernly includes a special 'flashrom' mode that allows for direct communication
with the flashrom program to manipulate the onboard SPI.  The protocol is
binary, and can be entered by issuing the following command:

    spi flashrom

Fernly will respond with a binary 0x05, indicating it is ready.

The format of the protocol is very simple.  The host writes the number of bytes
to write, then the number of bytes to read, and then writes the data to send
to the flash chip.  It then reads the requested number of bytes.  For
example, to send a 2-byte command '0xfe 0xfa' followed by a 3-byte response,
write the following data to the serial port:

| 02 03 fe fa |

Then read three bytes of data from the serial port.

A maximum of 255 bytes may be transmitted and received at one time, though
in practice these numbers may be smaller.

To exit 'spi flashrom' mode and return to fernly, read/write zero bytes.
That is, send the following packet:

| 00 00 |

See ROM-BACKUP.txt for user-level instructions how to backup/restore
FlashROM of your device.

Licensing
---------

Fernly is licensed under the BSD 2-clause license (see LICENSE).

Previous versions of fernly linked against division libraries taken from U-Boot,
which were licensed under GPL-2.  These files have been removed.

Instead, we supply a version of libgcc.a.  This file was extracted from a
standard gcc toolchain, specifically:

    https://code.google.com/p/yus-repo/downloads/detail?name=arm-none-eabi-4.6-armv5.tar.gz

It has not been modified, and its distribution here should be covered under
the "runtime exception".


Memory Map
----------

| 0x00000000 | 0x0fffffff | 0x0fffffff | PSRAM map, repeated and mirrored at 0x00800000 offsets               |
| ---------- | ---------- | ---------- | ----------------------------------- |
| 0x10000000 | 0x1fffffff | 0x0fffffff | Memory-mapped SPI chip              |
| ?????????? | ?????????? | ?????????? | ??????????????????????????????????? |
| 0x70000000 | 0x7000cfff |     0xcfff | On-chip SRAM (maybe cache?)         |
| 0x70008000 | 0x7000A400 | 0x00002400 | SRAM CODE area                      |
| 0x7000A400 | 0x7000B000 | 0x00000C00 | SRAM DATA area                      |
| ?????????? | ?????????? | ?????????? | ??????????????????????????????????? |
| 0x80000000 | 0x80000008 |       0x08 | Config block (chip version, etc.)   |
| 0x82000000 | 0x82d00000 | ?????????? | Modem system stuff                  |
| 0x83000000 | 0xa3090000 | ?????????? | Modem peripheral stuff              |
| 0x83020000 |            |            | TDMA unit                           |
| 0x83050000 |            |            | Frame Check Sequence unit           |
| 0x83060000 |            |            | GPRS cipher unit                    |
| 0x83070000 |            |            | Baseband serial interface           |
| 0x83080000 |            |            | Baseband parallel interface         |
| 0xa0000000 | 0xa0000008 |       0x08 | Config block (mirror?)              |
| 0xa0010000 | ?????????? | ?????????? | Power, config block                 |
| 0xa0020000 | 0xa0020e10 |     0x0e10 | GPIO control block                  |
| 0xa0030000 | 0xa0030040 |       0x40 | WDT block                           |
|            |            |            |   - 0x08 -> WDT register (?)        |
|            |            |            |   - 0x18 -> Boot src (?)            |
| 0xa0030800 | ?????????? | ?????????? | ????????????????????????????        |
| 0xa0040000 | ?????????? | ?????????? | ??????????????????????????????????? |
| 0xa0050000 | ?????????? | ?????????? | External memory block               |
| 0xa0060000 | ?????????? | ?????????? | IRQ Controller block                |
| 0xa0070000 | ========== | ========== | DMA Controller block                |
| 0xa0080000 | 0xa008005c |       0x5c | UART1 block                         |
| 0xa0090000 | 0xa009005c |       0x5c | UART2 block                         |
| 0xa00a0000 | 0xa00a0000 |       0x5c | UART3 block                         |
| 0xa00b0000 | 0xa00b006c |       0x6c | Bluetooth interface block           |
| 0xa00c0000 | 0xa00c002c |       0x2c | General purpose timer block         |
| 0xa00d0000 | 0xa00d0024 |       0x24 | Keypad scanner block                |
| 0xa00e0000 | 0xa00e0008 |       0x0c | PWM1 block                          |
| 0xa00f0000 | 0xa00f00b0 |       0xb0 | SIM1 interface block                |
| 0xa0100000 | 0xa01000b0 |       0xb0 | SIM2 interface block                |
| 0xa0110000 | ?????????? | ?????????? | SEJ/CHE (Security engine) block     |
| 0xa0120000 | 0xa0120074 |       0x74 | I2C block                           |
| 0xa0130000 | 0xa0130098 |       0x98 | SD1 block (MSDC)                    |
| 0xa0140000 | ?????????? | ?????????? | Serial flash block                  |
| 0xa0150000 | ?????????? | ?????????? | ?? MAYBE also SPI ????????????????? |
| 0xa0160000 | ?????????? | ?????????? | Die-to-die master interface         |
| 0xa0170000 | ?????????? | ?????????? | Analogue chip controller block      |
| 0xa0180000 | ?????????? | ?????????? | TOPSM block                         |
| 0xa0190000 | 0xa0190310 |       0x58 | HIF (DMA?) interface block          |
| 0xa01b0000 | 0xa01b0058 |       0x58 | NLI (arbiter) interface block       |
| 0xa01c0000 | ?????????? | ?????????? | EFuse block                         |
| 0xa01e0000 | ?????????? | ?????????? | SPI block                           |
| 0xa01f0000 | 0xa01f0060 |       0x60 | OS timer block                      |
| 0xa0210000 | ?????????? | ?????????? | More analog bits                    |
| 0xa0220000 | ?????????? | ?????????? | MBist block                         |
| 0xa0240000 | ?????????? | ?????????? | NAND flash block                    |
| 0xa0260000 | 0xa0260058 |       0x58 | FSPI (internal FM radio) block      |
| 0xa0270000 | 0xa0270098 |       0x98 | SD2 block                           |
| 0xa02A0000 | 0xa02A0074 |       0x74 | I2C2 block 1.8v                     |
| 0xa0400000 | ?????????? | ?????????? | IMGDMA block                        |
| 0xa0410000 | ?????????? | ?????????? | IDP RESZ CR2                        |
| 0xa0420000 | 0xa04201d8 |     0x01d8 | CAM interface block                 |
| 0xa0430000 | ?????????? | ?????????? | Serial camera block                 |
| 0xa0440000 | ?????????? | ?????????? | 2D graphics block                   |
| 0xa0450000 | ?????????? | ?????????? | LCD interface block                 |
| 0xa0460000 | ?????????? | ?????????? | Multimedia system BIST block        |
| 0xa0470000 | ?????????? | ?????????? | Multimedia colour config block      |
| 0xa0480000 | ?????????? | ?????????? | Multimedia system config block      |
| 0xa0500000 | ?????????? | ?????????? | ARM configuration block             |
| 0xa0510000 | ?????????? | ?????????? | Boot configuration block            |
| 0xa0520000 | ?????????? | ?????????? | Code decompression engine block     |
| 0xa0530000 | ?????????? | ?????????? | Level 1 cache block                 |
| 0xa0540000 | ?????????? | ?????????? | MPU config block                    |
| 0xa0700000 | ?????????? | ?????????? | Power management block. Write (val & 0xfe0f &#124; 0x140) to 0xa0700230 to power off. |
| 0xa0710000 | 0xa0710078 |       0x78 | RTC block                           |
| 0xa0720000 | ?????????? | ?????????? | Analogue baseband config block      |
| 0xa0730000 | 0xa0730100 |     ?????? | Analogue die config                 |
| 0xa0730104 | 0xa073104c |     ?????? | GPIO mode / pull control blocks     |
| 0xa074000c | 0xa0740014 |       0x0c | PWM2 block                          |
| 0xa0740018 | 0xa0740020 |       0x0c | PWM3 block                          |
| 0xa0750000 | 0xa075005c |       0x5c | ADCDET block                        |
| 0xa0760000 | ?????????? | ?????????? | Analogue IRQ controller             |
| 0xa0790000 | 0xa07900d8 |       0xd8 | ADC block                           |
| 0xa07a0000 | ?????????? | ?????????? | Analogue Die-to-die block           |
| 0xa0900000 | 0xa0900240 | ?????????? | USB block                           |
| 0xa0910000 | ?????????? | ?????????? | ??????????????????????????????????? |
| 0xa0920000 | ?????????? | ?????????? | AHB DMA block                       |
| 0xa3300000 | 0xa33a0000 |      0x6c? | Bluetooth things                    |
| 0xfff00000 | 0xffffffff |   0x100000 | Boot ROM, mirrored each 64K (its real size) |
