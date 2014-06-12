#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

int uart_putc(uint8_t c);
int uart_puts(char *s);

#endif /* __UART_H__ */
