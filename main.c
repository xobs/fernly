#include <string.h>
#include "serial.h"
#include "utils.h"
#include "bionic.h"

static inline void writeb(uint8_t value, uint32_t addr)
{
	*((volatile uint8_t *)addr) = value;
}

static inline uint8_t readb(uint32_t addr)
{
	return *(volatile uint8_t *)addr;
}

static inline void writew(uint16_t value, uint32_t addr)
{
	*((volatile uint16_t *)addr) = value;
}

static inline uint16_t readw(uint32_t addr)
{
	return *(volatile uint16_t *)addr;
}

static inline void writel(uint32_t value, uint32_t addr)
{
	*((volatile uint32_t *)addr) = value;
}

static inline uint32_t readl(uint32_t addr)
{
	return *(volatile uint32_t *)addr;
}

/*
static int wdt_kick(void)
{
	writel(0x1971, 0xa0030008);
	return 0;
}

static int wdt_reboot(void)
{
	writel(0x1209, 0xa003001c);
	return 0;
}
*/

static int list_registers(void)
{
	int var;

	serial_puts("Registers:\n");

	serial_puts("CPSR: ");
	asm volatile ("mrs %0, cpsr":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("SPSR: ");
	asm volatile ("mrs %0, spsr":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R0:   ");
	asm volatile ("mov %0, r0":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R1:   ");
	asm volatile ("mov %0, r1":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R2:   ");
	asm volatile ("mov %0, r2":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R3:   ");
	asm volatile ("mov %0, r3":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R4:   ");
	asm volatile ("mov %0, r4":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R5:   ");
	asm volatile ("mov %0, r5":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R6:   ");
	asm volatile ("mov %0, r6":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R7:   ");
	asm volatile ("mov %0, r7":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R8:   ");
	asm volatile ("mov %0, r8":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R9:   ");
	asm volatile ("mov %0, r9":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R10:  ");
	asm volatile ("mov %0, r10":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("FP:   ");
	asm volatile ("mov %0, r11":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("IP:   ");
	asm volatile ("mov %0, r12":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("SP:   ");
	asm volatile ("mov %0, r13":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("LR:  ");
	asm volatile ("mov %0, r14":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("PC:  ");
	asm volatile ("mov %0, r15":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	return 0;
}

static int enable_irq(void)
{
	register int var;
	asm volatile ("mrs %0, cpsr":"=r" (var));
	if (!(var & 0x80)) {
		serial_puts("Interrupts already enabled\n");
		return -1;
	}

//	serial_puts("Interrupts were disabled.  Re-enabling...\n");
	var &= ~0x80;
	var &= ~0x1f;
	var |= 0x10;
	asm volatile ("msr cpsr, %0":"=r" (var));

	return 0;
}

static int enable_fiq(void)
{
	register int var;
	asm volatile ("mrs %0, cpsr":"=r" (var));
	if (!(var & 0x40)) {
		serial_puts("FIQ already enabled\n");
		return -1;
	}

//	serial_puts("FIQ was disabled.  Re-enabling...\n");
	var &= ~0x40;
	asm volatile ("msr cpsr, %0":"=r" (var));

	return 0;
}

static int do_init(void)
{
	void *rv_start = (void *)0x88;
	void *rv_end = (void *)0x88 + 256;

	list_registers();
	serial_init();

	/* Kick WDT */
	writel(0x1971, 0xa0030008);

	/* Disable WDT */
	writel(0x2200, 0xa0030000);

	serial_puts("\n\nFernly shell\n");

	/* Copy exception vectors to address 0 */
	_memcpy((void *)0, rv_start, rv_end - rv_start);
	enable_irq();
	enable_fiq();

	return 0;
}

static inline int get_hex(int bytes)
{
	uint32_t word = 0;
	uint8_t buf;
	int i;

	if (bytes == 4)
		i = 28;
	else if (bytes == 2)
		i = 12;
	else
		i = 4;

	while (i >= 0) {
		buf = serial_getc();
		if (buf > 96)
			buf -= 87;
		else if (buf > 64)
			buf -= 55;
		else
			buf -= 48;
		word |= (buf << i);

		i -= 4;
	}
	return word;
}

int main(void)
{
	int buf;
	int size;
	uint32_t offset;
	uint32_t value;
	do_init();

	/* Protocol:
	   Stream is byte-oriented.  The following commands are known:

	   r - read an address
	   w - write to an address

	   Responses:

	   k - Ready for command
	   ? - Unknown command
	*/


	while (1) {
		serial_putc('k');
		buf = serial_getc();

		switch (buf) {
		/* Read.  Format: r[otf]aaaaaaaa
		   otf -> read One, Two, or Four bytes
		   a.. -> address to read
		   */
		case 'r':
			size = serial_getc();
			if (size == 'o') {
				offset = get_hex(4);
				value = readb(offset);
				serial_puth(value, 2);
			}
			else if (size == 't') {
				offset = get_hex(4);
				value = readw(offset);
				serial_puth(value, 4);
			}
			else {
				offset = get_hex(4);
				value = readl(offset);
				serial_puth(value, 8);
			}
			break;

		/* Write.  Format: w[otf]aaaaaaaavvvvvvvv
		   otf -> write One, Two, or Four bytes
		   a.. -> address to write
		   v.. -> value to write
		   */
		case 'w':
			size = serial_getc();
			if (size == 'o') {
				offset = get_hex(4);
				value = get_hex(1);
				writeb(value, offset);
				serial_puth(value, 2);
			}
			else if (size == 't') {
				offset = get_hex(4);
				value = get_hex(2);
				writew(value, offset);
				serial_puth(value, 4);
			}
			else {
				offset = get_hex(4);
				value = get_hex(4);
				writel(value, offset);
				serial_puth(value, 8);
			}
			break;

		case 'z': {
			uint32_t start;
			uint32_t end;

			start = get_hex(4);
			end = get_hex(4);
			while (start < end) {
				*((uint32_t *)start) = 0;
				start += 4;
			}
			}
			break;

		default:
			serial_putc('?');
			break;
		}
	}

	return 0;
}
