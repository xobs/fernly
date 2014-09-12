#include "scriptic.h"
#include "fernvale-spi.h"

	/* Enable block mode for SPI CS0 */
	sc_new "spi_run_cs0", 1, 0, 0

	/* Wait for channel 2 to be idle */
	sc_write32 SPI_CTRL3_CHANNEL2_MASK, SPI_CTRL3_CHANNEL2_MASK, SPI_CTRL3
	sc_read32 SPI_CTRL3_CHANNEL2_IDLE, SPI_CTRL3_CHANNEL2_IDLE, SPI_CTRL3
	sc_read32 SPI_CTRL1_BUS_IDLE, SPI_CTRL1_BUS_IDLE, SPI_CTRL1

	/* Turn on block mode */
        sc_write32 SPI_BLOCKMODE_ENABLE, SPI_BLOCKMODE_ENABLE, SPI_BLOCKMODE

	/* Trigger the transfer */
	sc_write32 \
		SPI_BLOCKMODE_TRIGGER | SPI_BLOCKMODE_CS0, \
		SPI_BLOCKMODE_TRIGGER | SPI_BLOCKMODE_CS0, \
		SPI_BLOCKMODE

        /* Wait for the device to prepare itself */
	sc_read32 SPI_BLOCKMODE_PREP, SPI_BLOCKMODE_PREP, SPI_BLOCKMODE

	/* Wait for it to finish */
	sc_read32 0, SPI_BLOCKMODE_BUSY, SPI_BLOCKMODE

	/* Stop the transfer */
	sc_write32 0, SPI_BLOCKMODE_TRIGGER | SPI_BLOCKMODE_CS0, SPI_BLOCKMODE

	/* Wait for it to be disabled */
	sc_read32 0, SPI_BLOCKMODE_PREP, SPI_BLOCKMODE

	/* Exit block mode */
	sc_write32 0, SPI_BLOCKMODE_ENABLE, SPI_BLOCKMODE

	/* Wait for the chip to actually exit block mode */
	sc_read32 0, SPI_BLOCKMODE_ENABLE, SPI_BLOCKMODE

	/* Allow channel 2 to contain data */
	sc_write32 0, SPI_CTRL3_CHANNEL2_MASK, SPI_CTRL3

	sc_end


	/* Enable block mode for SPI CS1 */
	sc_new "spi_run_cs1", 1, 0, 0

	/* Wait for channel 2 to be idle */
	sc_write32 SPI_CTRL3_CHANNEL2_MASK, SPI_CTRL3_CHANNEL2_MASK, SPI_CTRL3
	sc_read32 SPI_CTRL3_CHANNEL2_IDLE, SPI_CTRL3_CHANNEL2_IDLE, SPI_CTRL3
	sc_read32 SPI_CTRL1_BUS_IDLE, SPI_CTRL1_BUS_IDLE, SPI_CTRL1

	/* Turn on block mode */
        sc_write32 SPI_BLOCKMODE_ENABLE, SPI_BLOCKMODE_ENABLE, SPI_BLOCKMODE

	/* Trigger the transfer */
	sc_write32 \
		SPI_BLOCKMODE_TRIGGER | SPI_BLOCKMODE_CS1, \
		SPI_BLOCKMODE_TRIGGER | SPI_BLOCKMODE_CS1, \
		SPI_BLOCKMODE

        /* Wait for the device to prepare itself */
	sc_read32 SPI_BLOCKMODE_PREP, SPI_BLOCKMODE_PREP, SPI_BLOCKMODE

	/* Wait for it to finish */
	sc_read32 0, SPI_BLOCKMODE_BUSY, SPI_BLOCKMODE

	/* Stop the transfer */
	sc_write32 0, SPI_BLOCKMODE_TRIGGER | SPI_BLOCKMODE_CS1, SPI_BLOCKMODE

	/* Wait for it to be disabled */
	sc_read32 0, SPI_BLOCKMODE_PREP, SPI_BLOCKMODE

	/* Exit block mode */
	sc_write32 0, SPI_BLOCKMODE_ENABLE, SPI_BLOCKMODE

	/* Wait for the chip to actually exit block mode */
	sc_read32 0, SPI_BLOCKMODE_ENABLE, SPI_BLOCKMODE

	/* Allow channel 2 to contain data */
	sc_write32 0, SPI_CTRL3_CHANNEL2_MASK, SPI_CTRL3

	sc_end
