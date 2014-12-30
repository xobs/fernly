#include <stdint.h>
#include <string.h>
#include "printf.h"
#include "bionic.h"
#include "memio.h"
#include "spi.h"
#include "fernvale-spi.h"
#include "scriptic.h"

static void spi_init(void)
{
	static int spi_initted = 0;

	if (spi_initted)
		return;

	spi_initted = 1;
	scriptic_run("spi_init");
}

int spi_cmd_txrx(uint8_t tx_size, uint8_t rx_size,
		 uint8_t *tx_buf, uint8_t *rx_buf)
{
	spi_init();

	memcpy(SPI_DATA, tx_buf, tx_size);

	writel(tx_size, SPI_WRITE_COUNT);
	writel(rx_size, SPI_READ_COUNT);

	scriptic_run("spi_run");

	memcpy(rx_buf, SPI_DATA + tx_size, rx_size);

	return 0;
}
