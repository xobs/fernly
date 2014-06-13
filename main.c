#include <string.h>
#include "serial.h"
#include "utils.h"
#include "bionic.h"

#define PROMPT "fernly> "

static int serial_get_line(uint8_t *bfr, int len)
{
	int cur = 0;

	while (cur < len) {
		bfr[cur] = serial_getc();
		serial_putc(bfr[cur]);

		/* Carriage Return */
		if (bfr[cur] == '\n') {
			bfr[cur] = '\0';
			return 0;
		}

		/* Linefeed */
		else if (bfr[cur] == '\r') {
			bfr[cur] = '\0';
			return 0;
		}

		/* Backspace */
		else if (bfr[cur] == 0x7f) {
			bfr[cur] = '\0';

			if (cur > 0) {
				serial_putc('\b');
				serial_putc(' ');
				serial_putc('\b');
				cur--;
			}
		}

		/* Ctrl-U */
		else if (bfr[cur] == 0x15) {
			while (cur > 0) {
				serial_putc('\b');
				serial_putc(' ');
				serial_putc('\b');
				bfr[cur] = '\0';
				cur--;
			}
		}

		/* Ctrl-W */
		else if (bfr[cur] == 0x17) {
			while (cur > 0 && bfr[cur] != ' ') {
				serial_putc('\b');
				serial_putc(' ');
				serial_putc('\b');
				bfr[cur] = '\0';
				cur--;
			}
		}

		/* Escape code */
		else if (bfr[cur] == 0x1b) {
			/* Next two characters are escape codes */
			uint8_t next = serial_getc();
			/* Sanity check: next should be '[' */

			next = serial_getc();
		}
		else
			cur++;
	}
	bfr[len - 1] = '\0';
	return -1;
}

static void writew(uint16_t value, uint32_t addr)
{
	*((volatile uint16_t *)addr) = value;
}

static uint16_t readw(uint32_t addr)
{
	return *(volatile uint16_t *)addr;
}

static void writel(uint32_t value, uint32_t addr)
{
	*((volatile uint32_t *)addr) = value;
}

static uint32_t readl(uint32_t addr)
{
	return *(volatile uint32_t *)addr;
}

static int memory_test(uint32_t start, uint32_t length)
{
	int addr;
	register uint32_t val = 0xdeadbeef;
	register uint32_t test;
	uint32_t end = start + length;

	serial_puts("Testing memory from ");
	serial_puth(start, 8);
	serial_puts(" to ");
	serial_puth(end, 8);
	serial_puts("");

	for (addr = start; addr < end; addr += 4) {
		if (! (addr & 0xffff)) {
			serial_puts("\nAddress ");
			serial_puth(addr, 8);
			serial_puts("...");
		}

		writel(val, addr);
		test = readl(addr);
		if (test != val) {
			serial_puts("Address ");
			serial_puth(addr, 8);
			serial_puts(" appears to be invalid!  Expected 0xdeadbeef, got 0x");
			serial_puth(test, 8);
			serial_puts("\n");
		}
	}

	return 0;
}

static int wdt_kick(void)
{
	writel(0x1971, 0xa0030008);
	return 0;
}

static int memory_search(uint32_t start, uint32_t length, uint32_t nonce, uint32_t mask)
{
	int addr;
	register uint32_t test;
	uint32_t end = start + length;

	serial_puts("Looking in memory from ");
	serial_puth(start, 8);
	serial_puts(" to ");
	serial_puth(end, 8);
	serial_puts(" for nonce ");
	serial_puth(nonce, 8);
	serial_puts("\n");

	for (addr = start; addr < end; addr += 0x800) {
	//	if (! (addr & 0xffff)) {
			serial_puts("\rAddress ");
			serial_puth(addr, 8);
			serial_puts("...");
			wdt_kick();
	//	}

		test = readw(addr);
		if ((test & mask) == nonce) {
			serial_puts("    ");
			serial_puth(addr, 8);
			serial_puts(" matches nonce\n");
		}
	}

	return 0;
}

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

	serial_puts("R14:  ");
	asm volatile ("mov %0, r14":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("R15:  ");
	asm volatile ("mov %0, r15":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("LR:   ");
	asm volatile ("mov %0, lr":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	return 0;
}

static int read_address(uint8_t *arg)
{
	uint32_t addr = _strtoul(arg, 0, 16);
	serial_puth(addr, 16);
	serial_puts(": ");
	serial_puth(readl(addr), 16);
	serial_puts("\n");
	return 0;
}

static int write_address(uint8_t *arg)
{
	uint8_t *valpos;
	uint32_t val;
	uint32_t addr = _strtoul(arg, (void **)&valpos, 16);
	val = _strtoul(valpos, 0, 16);
	serial_puth(addr, 16);
	serial_puts(": ");
	serial_puth(val, 16);
	serial_puts("\n");
	writel(val, addr);
	return 0;
}

static int hex_print(uint8_t *arg)
{
	uint32_t *addr = (uint32_t *)_strtoul(arg, 0, 16);
	uint32_t data[1024];
	int i;
	for (i = 0; i < ((sizeof(data) / sizeof(*data))); i++)
		data[i] = addr[i];

	serial_puth((uint32_t)addr, 8);
	serial_puts(":\n");
	serial_print_hex(data, sizeof(data));
	serial_puts("\n");
	return 0;
}

int main(int argc, char **argv)
{
	serial_init();
	serial_puts("\n\nFernly shell\n");

	while (1) {
		uint8_t line[256];
		serial_puts(PROMPT);
		serial_get_line(line, sizeof(line));
		serial_puts("\n");

		switch(*line) {
			case ' ':
			case '\0':
				break;

			case 'm':
				/* 0x70000000 - 0x7000ffff -> Stack, OCRAM? */
				memory_test(0x0ffe0000, 0x40000000);
				break;

			case 's':
				memory_search(0xa0000020, 0x2f000000,
//					(0x2f << 24) | (0x2e << 16) |
//					(0x2d << 8)  | (0x2c << 0),

//					(0x0b << 24) | (0x0a << 16) |
//					(0x09 << 8)  | (0x08 << 0),

					0x00000401,
					0x0000ffff);
				break;

			case 'h':
				hex_print(line + 1);
				break;

			case 'l':
				list_registers();
				break;

			case 'r':
				read_address(line + 1);
				break;

			case 'w':
				write_address(line + 1);
				break;

			case 'd':
				serial_puts("Kicking WDT...\n");
				wdt_kick();
				break;

			default:
				serial_puts("Unknown command\n");
				break;
		}
	}

	return 0;
}
