#include <stdint.h>
#include "serial.h"

int uart_putc(uint8_t c) {
	uint32_t *uart_ldr = (uint32_t *)0xa0080014;
	uint32_t *uart_sbr = (uint32_t *)0xa0080000;
	/* Wait for UART to be empty */
	while (! (*uart_ldr & 0x20));
	*uart_sbr = c;
	return 0;
}

int uart_puts(char *s) {
	while(*s) {
		if (*s == '\n')
			uart_putc('\r');
		uart_putc(*s++);
	}
	return 0;
}
