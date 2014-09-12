#include <string.h>
#include "bionic.h"
#include "serial.h"
#include "printf.h"
#include "spi.h"

#define SPI_CSEL 0

int cmd_spi(int argc, char **argv)
{
	uint32_t recv_bytes_count;
	uint8_t xmit_bytes[argc - 1];
	int i;

	if (argc == 1 && !_strcasecmp(argv[0], "loop")) {
		printf("Repeatedly sending 0x9f -- id\n");
		while(1) {
			uint8_t out[1] = {0x9f};
			uint8_t in[3];
			spi_cmd_txrx(0, sizeof(out), sizeof(in), out, in);
			printf("ID: ");
			serial_print_hex(in, sizeof(in));
		}
	}

	if (argc < 2) {
		printf("Send SPI bytes out and read the response.\n");
		printf("Usage: spi [count] [byte 1] [byte 2] ...\n");
		printf("    Where \"count\" is the number of bytes to expect"
			" in response.\n");
		return 1;
	}

	recv_bytes_count = _strtoul(argv[0], NULL, 0);
	uint8_t recv_bytes[recv_bytes_count];

	for (i = 1; i < argc; i++)
		xmit_bytes[i - 1] = _strtoul(argv[i], NULL, 0);

	printf("Transmitting %d bytes and expecting a response with %d bytes:\n",
			sizeof(xmit_bytes), sizeof(recv_bytes));
	serial_print_hex(xmit_bytes, sizeof(xmit_bytes));

	spi_cmd_txrx(SPI_CSEL, sizeof(xmit_bytes), sizeof(recv_bytes),
			xmit_bytes, recv_bytes);

	printf("Response:\n");
	serial_print_hex(recv_bytes, sizeof(recv_bytes));

	return 0;
}
