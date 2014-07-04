#!/bin/sh
make && sudo radare2 -c 's 0x3460; wf .//build/firmware.bin; !echo 0 > /sys/class/gpio/gpio17/value; !sleep 0.05; !echo 1 > /sys/class/gpio/gpio17/value' fv://
