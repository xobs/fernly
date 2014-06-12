Fernly - Fernvale Reversing OS
========================================

Fernly is a simple operating system designed for use in the reverse engineering
of the Fernvale CPU.  It will likely be disposed of when the system has been
understood well enough to implement a full operating system.


Usage
-----

To compile, simply run "make".

To install, use radare2:

    $ sudo radare2 fv://
    [0x00000000]> s 0x3460
    [0x00003460]> wf .//build/firmware.bin 

