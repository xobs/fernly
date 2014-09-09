#include <stdint.h>
#include "serial.h"
#include "printf.h"

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

int serial_print_hex_offset(const void *block, int count, int offset)
{
	int byte;
	const uint8_t *b = block;
	count += offset;
	b -= offset;
	for ( ; offset < count; offset += 16) {
		serial_puth(offset, 8);

		for (byte = 0; byte < 16; byte++) {
			if (byte == 8)
				serial_putc(' ');
			serial_putc(' ');
			if (offset + byte < count)
				serial_puth(b[offset + byte] & 0xff, 2);
			else
				serial_puts("  ");
		}

		serial_puts("  |");
		for (byte = 0; byte < 16 && byte + offset < count; byte++)
			serial_putc(_isprint(b[offset + byte]) ?
					b[offset + byte] :
					'.');
			serial_puts("|\n");
	}
	return 0;
}

int serial_print_hex(const void *block, int count)
{
	return serial_print_hex_offset(block, count, 0);
}

uint32_t __div64_32(uint64_t *n, uint32_t base)
{
	uint64_t rem = *n;
	uint64_t b = base;
	uint64_t res, d = 1;
	uint32_t high = rem >> 32;

	/* Reduce the thing a bit first */
	res = 0;
	if (high >= base) {
		high /= base;
		res = (uint64_t) high << 32;
		rem -= (uint64_t) (high*base) << 32;
	}

	while ((int64_t)b > 0 && b < rem) {
		b = b+b;
		d = d+d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}
		b >>= 1;
		d >>= 1;
	} while (d);

	*n = res;
	return rem;
}

void __div0 (void)
{
	printf("Division by 0.  Hanging.\n");
	while(1);
}
