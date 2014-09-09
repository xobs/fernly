#include <stdint.h>
#include "serial.h"

#define PUTS_HANDLE_NEWLINE
#define SERIAL_USB

#ifdef SERIAL_UART

#define UART_IS_DLL 0x100
#define UART_IS_LCR 0x200

#define UART_RBR 0x00
#define UART_THR 0x00
#define UART_IER 0x04
#define UART_IIR 0x08
#define UART_FCR 0x08
#define UART_LCR 0x0c
#define UART_MCR 0x10
#define UART_LSR 0x14
#define UART_MSR 0x18
#define UART_SCR 0x1c

#define UART_SPEED 0x24

/* The following are active when LCR[7] = 1 */
#define UART_DLL 0x100
#define UART_DLH 0x104

/* The following are active when LCR = 0xbf */
#define UART_EFR   0x208
#define UART_XON1  0x210
#define UART_XON2  0x214
#define UART_XOFF1 0x218
#define UART_XOFF2 0x21c

#define UART_BASE 0xa0080000

enum uart_baudrate {
	UART_38400,
	UART_57600,
	UART_115200,
	UART_230400,
	UART_460800,
	UART_614400,
	UART_921600,
};
#define UART_BAUD_RATE UART_115200

#if 0
/* 26MHz clock input (used when no PLL initialized directly after poweron) */
static const uint16_t divider[] = {
	[UART_38400]    = 42,
	[UART_57600]    = 28,
	[UART_115200]   = 14,
	[UART_230400]   = 7,
	[UART_460800]   = 14,   /* would need UART_REG(HIGHSPEED) = 1 or 2 */
	[UART_921600]   = 7,    /* would need UART_REG(HIGHSPEED) = 2 */
};
#else
/* 52MHz clock input (after PLL init) */
static const uint16_t divider[] = {
	[UART_38400]    = 85,
	[UART_57600]    = 56,
	[UART_115200]   = 28,
	[UART_230400]   = 14,
	[UART_460800]   = 7,
	[UART_921600]   = 7,    /* would need UART_REG(HIGHSPEED) = 1 */
};
#endif

static uint8_t uart_getreg(int regnum)
{
	volatile uint32_t *reg = (uint32_t *)(UART_BASE + (regnum & 0xff));
	return *reg;
}

static void uart_setreg(int regnum, uint8_t val)
{
	uint8_t old_lcr;
	if (regnum & UART_IS_DLL)
		uart_setreg(UART_LCR, uart_getreg(UART_LCR) | 0x80);
	else if (regnum & UART_IS_LCR) {
		old_lcr = uart_getreg(UART_LCR);
		uart_setreg(UART_LCR, 0xbf);
	}

	volatile uint32_t *reg = (uint32_t *)(UART_BASE + (regnum & 0xff));
	*reg = val;

	if (regnum & UART_IS_DLL)
		uart_setreg(UART_LCR, uart_getreg(UART_LCR) & ~0x80);
	else if (regnum & UART_IS_LCR)
		uart_setreg(UART_LCR, old_lcr);
}

int serial_putc(uint8_t c)
{
	/* Wait for UART to be empty */
	while (! (uart_getreg(UART_LSR) & 0x20));

	uart_setreg(UART_RBR, c);
	return 0;
}

uint8_t serial_getc(void)
{
	while (! (uart_getreg(UART_LSR) & 0x01));
	return uart_getreg(UART_RBR);
}

int serial_puts(const void *s)
{
	const char *str = s;
	while(*str) {
		if (*str == '\n')
			serial_putc('\r');
		serial_putc(*str++);
	}
	return 0;
}

void serial_init(void)
{
	int tmp;

	// Setup 8-N-1,(UART_WLS_8 | UART_NONE_PARITY | UART_1_STOP) = 0x03
	uart_setreg(UART_LCR, 0x03);

	// Set BaudRate 
	// config by UART_BAUD_RATE(9:115200)
	uart_setreg(UART_DLL, divider[UART_BAUD_RATE]&0xff);
	uart_setreg(UART_DLH, divider[UART_BAUD_RATE]>>8);
	uart_setreg(UART_LCR, 0x03);

	// Enable Fifo, and Rx Trigger level = 16bytes, flush Tx, Rx fifo
	// (UART_FCR_FIFOEN | UART_FCR_4Byte_Level | UART_FCR_RFR | UART_FCR_TFR) = 0x47
	uart_setreg(UART_FCR, 0x47);

	// DTR , RTS is on, data will be coming,Output2 is high
	uart_setreg(UART_MCR, 0x03);

	/* Set up normal interrupts */
	uart_setreg(UART_IER, 0x0d);

	/* Pause a while */
	for (tmp=0; tmp<0xff; tmp++);
}
#else /* SERIAL_USB */

static void (*rom_usb_read)(void *data, int bytes, int timeout) = (void *)0xfff03639;
static void (*rom_usb_write)(const void *data, int bytes, int timeout) = (void *)0xfff03653;
static void (*rom_usb_flush)(void) = (void *)0xfff04845;

int serial_putc(uint8_t c)
{
	if (c == '\n')
		serial_putc('\r');
	rom_usb_write(&c, 1, -1);
	rom_usb_flush();
	return 0;
}

uint8_t serial_getc(void)
{
	uint8_t bfr;
	rom_usb_read(&bfr, 1, -1);
	return bfr;
}

int serial_puts(const void *s)
{
#ifdef PUTS_HANDLE_NEWLINE
	const char *str = s;
	while(*str) {
		if (*str == '\n')
			serial_putc('\r');
		serial_putc(*str++);
	}
#else
	uint32_t len = _strlen(s);
	rom_usb_write(s, len, -1);
	rom_usb_flush();
#endif
	return 0;
}

void serial_init(void)
{
}

#endif /* UART */
