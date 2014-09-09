#include "scriptic.h"
#include "fernvale-pll.h"

	sc_new "set_plls", 1, 0, 0

	sc_write16 0, 0, PLL_CTRL_CON2
	sc_write16 0, 0, PLL_CTRL_CON3
	sc_write16 0, 0, PLL_CTRL_CON0
	sc_usleep 1

	sc_write16 1, 1, PLL_CTRL_UPLL_CON0
	sc_write16 0x1840, 0, PLL_CTRL_EPLL_CON0
	sc_write16 0x100, 0x100, PLL_CTRL_EPLL_CON1
	sc_write16 1, 0, PLL_CTRL_MDDS_CON0
	sc_write16 1, 1, PLL_CTRL_MPLL_CON0
	sc_usleep 1

	sc_write16 1, 0, PLL_CTRL_EDDS_CON0
	sc_write16 1, 1, PLL_CTRL_EPLL_CON0
	sc_usleep 1

	sc_write16 0x4000, 0x4000, PLL_CTRL_CLK_CONDB
	sc_usleep 1

	sc_write32 0x8048, 0, PLL_CTRL_CLK_CONDC
	/* Run the SPI clock at 104 MHz */
	sc_write32 0xd002, 0, PLL_CTRL_CLK_CONDH
	sc_write32 0xb6a0, 0, PLL_CTRL_CLK_CONDC
	sc_end	
