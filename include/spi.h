
#ifndef __SPI_H__
#define __SPI_H__

int spi_cmd_txrx(uint8_t tx_size, uint8_t rx_size,
		 uint8_t *tx_buf, uint8_t *rx_buf);

#endif /* __SPI_H__ */
