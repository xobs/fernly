#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>

int serial_putc(uint8_t c);
int serial_puts(void *s);
void serial_puth(uint32_t hex, int digits); /* Put hex */

uint8_t serial_getc(void);
void serial_init(void);

int serial_print_hex(void *bfr, int count);

#endif /* __SERIAL_H__ */
