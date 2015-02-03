#include <stdint.h>
#include "serial.h"
#include "memio.h"

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
	while (! (uart_getreg(UART_LSR) & 0x20))
		asm("");

	uart_setreg(UART_RBR, c);
	return 0;
}

uint8_t serial_getc(void)
{
	while (! (uart_getreg(UART_LSR) & 0x01))
		asm("");
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

int serial_read(void *data, int bytes)
{
	while (--bytes)
		((uint8_t *)data++) = serial_getc();
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

#include "fernvale-usb.h"

static volatile uint8_t *recv_bfr = (uint8_t *)0x70000000;
static int recv_size = 0;
static int recv_offset = 0;

#pragma GCC optimize ("-O1")

static void usb_receive_wait(uint8_t endpoint_number)
{
	/* Wait for data to exist, ignoring other USB IRQs */
	while (!readb(USB_CTRL_INTROUT))
		(void)readb(USB_CTRL_INTRUSB);

	/* Select EP1 */
	writeb(endpoint_number, USB_CTRL_INDEX);

	/* Set endpoint to OUT */
	writeb(readb(USB_CTRL_EP_INCSR2) & ~USB_CTRL_EP_INCSR2_MODE,
			USB_CTRL_EP_INCSR2);

	while (!(readb(USB_CTRL_EP_OUTCSR1) & USB_CTRL_EP_OUTCSR1_RXPKTRDY))
		asm("");
		
	recv_size  = (readb(USB_CTRL_EP_COUNT1) << 0) & 0x00ff;
	recv_size |= (readb(USB_CTRL_EP_COUNT2) << 8) & 0x0300;

	/* Fill in the receive buffer */
	for (recv_offset = 0; recv_offset < recv_size; recv_offset++)
		recv_bfr[recv_offset] = readb(USB_CTRL_EP1_FIFO_DB0);
	recv_offset = 0;

	/* Clear FIFO (write 0 to RXPKTRDY) */
	writeb(0, USB_CTRL_EP_OUTCSR1);
}

static void usb_handle_irqs(int epnum)
{
	/* Ignore general-purpose IRQs */
	(void)readb(USB_CTRL_INTRUSB);

	/* Select EP1 */
	writeb(epnum, USB_CTRL_INDEX);

	/* Flush the output FIFO if there is data present */
	if (readb(USB_CTRL_EP_INCSR1) & USB_CTRL_EP_INCSR1_FIFONOTEMPTY) {

		/* Set endpoint to IN */
		writeb(readb(USB_CTRL_EP_INCSR2) | USB_CTRL_EP_INCSR2_MODE,
			USB_CTRL_EP_INCSR2);

		writeb(USB_CTRL_EP_INCSR1_INPKTRDY, USB_CTRL_EP_INCSR1);
	}

	/* Set endpoint to OUT */
	writeb(readb(USB_CTRL_EP_INCSR2) & ~USB_CTRL_EP_INCSR2_MODE,
			USB_CTRL_EP_INCSR2);

	/* If there are incoming bytes, read them into the buffer */
	if (readb(USB_CTRL_EP_OUTCSR1) & USB_CTRL_EP_OUTCSR1_RXPKTRDY)
		usb_receive_wait(epnum);

	/* Set endpoint to IN mode */
	writeb(readb(USB_CTRL_EP_INCSR2) | USB_CTRL_EP_INCSR2_MODE,
			USB_CTRL_EP_INCSR2);
}

int serial_putc(uint8_t c)
{
	/* Fix up linefeeds */
	if (c == '\n')
		serial_putc('\r');

	/* Wait for the bus to be idle, so we don't double-xmit */
	while (readb(USB_CTRL_INTRIN))
		asm("");

	/* Select EP1 */
	writeb(1, USB_CTRL_INDEX);

	/* Add the character to the FIFO */
	writeb(c, USB_CTRL_EP1_FIFO_DB0);
	writeb(USB_CTRL_EP_INCSR1_INPKTRDY, USB_CTRL_EP_INCSR1);

	/* Wait for the character to transmit, so we don't double-xmit */
	while (!readb(USB_CTRL_INTRIN))
		asm("");

	return 0;
}

uint8_t serial_getc(void)
{
	/* Wait for data if the buffer is empty */
	while (recv_offset == recv_size)
		usb_handle_irqs(1);

	return recv_bfr[recv_offset++];
}

int serial_puts(const void *s)
{
	const char *str = s;
	while(*str) {
		serial_putc(*str++);
	}
	return 0;
}

int serial_read(void *data, int bytes)
{
	while (--bytes) {
		*((uint8_t *)data) = serial_getc();
		data++;
	}

	return 0;
}

void serial_init(void)
{
	(void)readb(USB_CTRL_INTROUT);
	(void)readb(USB_CTRL_INTRIN);
	(void)readb(USB_CTRL_INTRUSB);

	writeb(0, USB_CTRL_INTROUTE);
	(void)readb(USB_CTRL_INTROUTE);

	writeb(0, USB_CTRL_INTRINE);
	(void)readb(USB_CTRL_INTRINE);

	writeb(USB_CTRL_INTROUTE_EP1_OUT_ENABLE, USB_CTRL_INTROUTE);
	writeb(USB_CTRL_INTRINE_EP1_IN_ENABLE | USB_CTRL_INTRINE_EP0_ENABLE,
			USB_CTRL_INTRINE);

	/* Select EP1 */
	writeb(1, USB_CTRL_INDEX);

	/* Clear FIFO */
	writeb(USB_CTRL_EP_INCSR1_FLUSHFIFO, USB_CTRL_EP_OUTCSR1);

	/* Clear the second packet */
	writeb(USB_CTRL_EP_INCSR1_FLUSHFIFO, USB_CTRL_EP_OUTCSR1);

	recv_offset = 0;
	recv_size = 0;
	usb_handle_irqs(1);
}

#endif /* !SERIAL_UART */
