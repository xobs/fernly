#!/bin/bash

echo "splashscreen.sh <imagefile>"
echo Converting any 240x320 image to PNM
convert $1 picture.pnm
echo Converting from PNM to Fernvale Framebuffer format
perl ./pnm2bin.pl picture.pnm picture.bin
echo Telling fernvale to load the image
echo "load 0x40000 153600" >/dev/fernvale
echo Loading the image
cat picture.bin >/dev/fernvale
echo Displaying image
echo "lcd run" >/dev/fernvale
echo Done
