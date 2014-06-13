#include <stdint.h>
#include "serial.h"

static inline int _isprint(char c)
{
	return c > 32 && c < 127;
}

#define HEX_CHARS "0123456789abcdef"

void serial_puth(uint32_t pair, int digits)
{
	if (digits >= 8)
		serial_putc(HEX_CHARS[(pair >> 28) & 0xf]);
	if (digits >= 7)
		serial_putc(HEX_CHARS[(pair >> 24) & 0xf]);
	if (digits >= 6)
		serial_putc(HEX_CHARS[(pair >> 20) & 0xf]);
	if (digits >= 5)
		serial_putc(HEX_CHARS[(pair >> 16) & 0xf]);
	if (digits >= 4)
		serial_putc(HEX_CHARS[(pair >> 12) & 0xf]);
	if (digits >= 3)
		serial_putc(HEX_CHARS[(pair >> 8) & 0xf]);
	if (digits >= 2)
		serial_putc(HEX_CHARS[(pair >> 4) & 0xf]);
	if (digits >= 1)
		serial_putc(HEX_CHARS[(pair >> 0) & 0xf]);
}

int serial_print_hex_offset(uint8_t *block, int count, int offset)
{
	int byte;
	count += offset;
	block -= offset;
	for ( ; offset < count; offset += 16) {
		serial_puth(offset, 8);

		for (byte = 0; byte < 16; byte++) {
			if (byte == 8)
				serial_putc(' ');
			serial_putc(' ');
			if (offset + byte < count)
				serial_puth(block[offset + byte] & 0xff, 2);
			else
				serial_puts("  ");
		}

		serial_puts("  |");
		for (byte = 0; byte < 16 && byte + offset < count; byte++)
			serial_putc(_isprint(block[offset + byte]) ?
					block[offset + byte] :
					'.');
			serial_puts("|\n");
	}
	return 0;
}

int serial_print_hex(void *block, int count)
{
	return serial_print_hex_offset(block, count, 0);
}

