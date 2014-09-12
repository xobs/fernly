#include "scriptic.h"
#include "fernvale-spi.h"

	sc_new "spi_init", 1, 0, 0

	/* Ungate the clock */
	sc_write16 0x800, 0x800, 0x80000320

	/* Switch to SPI NOR */ /* (XXX Might need to invert this) */
	sc_write16 0, 1, 0x80000230

	/* Remap GPIOs to be SPI */
	sc_write32 0x11111100, 0x77777700, 0xa0020c80
	sc_write32 0x00000011, 0x00000077, 0xa0020c90

	/* Enable high-impedence for GPIO mode */
	sc_write16 0, 0x3fff, 0xa0020b10
	sc_write16 0, 0x3fff, 0xa0020b40
	sc_write16 0, 0xdff1, 0xa0020b20
	sc_write16 0, 0xdff1, 0xa0020b50

	sc_end
