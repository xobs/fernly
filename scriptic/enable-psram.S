#include "scriptic.h"
#include "fernvale-emi.h"

	sc_new "enable_psram", 1, 0, 0

	/* Unclear what these do, but required on some chips */
	sc_write32 0x39320d61, 0, EMI_CTRL_CONB
	sc_write32 0x400, 0x400, EMI_CTRL_CONF

	sc_write32 0x401, 0, EMI_CTRL_GENA

	/* Remap EMI to 0x10000000, and SPI to 0x00000000 */
	sc_write32 2, 0, EMI_CTRL_REMAP

	/* Memory configuration */
	sc_read16 0, 0, 0x1ffffffe
	sc_usleep 50
	sc_read16 0, 0, 0x1ffffffe
	sc_usleep 50

	sc_write16 1, 0, 0x1ffffffe
	sc_usleep 50
	sc_write16 0x2b13, 0, 0x1ffffffe
	sc_usleep 50

	sc_read16 0, 0, 0x1ffffffe
	sc_usleep 50
	sc_read16 0, 0, 0x1ffffffe
	sc_usleep 50

	sc_write16 0, 0, 0x1ffffffe
	sc_usleep 50
	sc_write16 0x10, 0, 0x1ffffffe
	sc_usleep 50

	sc_write32 0xa0000000, 0, EMI_CTRL_CONB
	sc_write32 0xb2024419, 0, EMI_CTRL_CONF
	sc_usleep 50

	sc_write32 0x400, 0x400, EMI_CTRL_GENA

	/* Now map EMI back to 0x00000000, and SPI to 0x00000000 */
	sc_write32 3, 0, EMI_CTRL_REMAP
	sc_usleep 50

	sc_write32 0x20004001, 0, EMI_CTRL_RDCT
	sc_write32 0x5111, 0, EMI_CTRL_DSRAM

	sc_write32 0, 0, EMI_CTRL_IDLA
	sc_write32 0, 0, EMI_CTRL_IDLB
	sc_write32 0, 0, EMI_CTRL_IDLC
	sc_write32 0, 0, EMI_CTRL_IDLD
	sc_write32 0, 0, EMI_CTRL_IDLE
	sc_write32 0, 0, EMI_CTRL_ODLA
	sc_write32 0, 0, EMI_CTRL_ODLB
	sc_write32 0, 0, EMI_CTRL_ODLC
	sc_write32 0, 0, EMI_CTRL_ODLD
	sc_write32 0, 0, EMI_CTRL_ODLE

	sc_write32 0x00010001, 0, EMI_CTRL_IOA
	sc_write32 0x00010001, 0, EMI_CTRL_IOB

	sc_usleep 50

	/* Calibrate DQ in delay */
	sc_call calibrate_psram, 0

	sc_write32 0x300f0000, 0, EMI_CTRL_DLLV
	sc_read32 0x80, 0x80, EMI_CTRL_DLLV
	sc_write32 0x700f0000, 0, EMI_CTRL_DLLV
	sc_read32 0x80, 0x00, EMI_CTRL_DLLV
	sc_write32 0x100f0000, 0, EMI_CTRL_DLLV

	sc_write32 0x5426, 0, EMI_CTRL_ARBA
	sc_write32 0x5009, 0, EMI_CTRL_ARBB
	sc_write32 0x5051, 0, EMI_CTRL_ARBC

	sc_end
