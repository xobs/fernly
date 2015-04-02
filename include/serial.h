#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>

int serial_putc(uint8_t c);
int serial_puts(const void *s);
void serial_puth(uint32_t hex, int digits); /* Put hex */
void serial_write(const void *d, int bytes);

uint8_t serial_getc(void);
int serial_available(void);
int serial_print_hex(const void *bfr, int count);
int serial_read(void *data, int bytes);

void serial_init(void);

#endif /* __SERIAL_H__ */
