#include <string.h>
#include "serial.h"
#include "utils.h"
#include "bionic.h"

//#define AUTOMATED

#if defined(AUTOMATED)
#define PROMPT "fernly> \n"
#else
#define PROMPT "fernly> "
#endif

static int serial_get_line(uint8_t *bfr, int len)
{
	int cur = 0;

	while (cur < len) {
		bfr[cur] = serial_getc();
#if !defined(AUTOMATED)
		serial_putc(bfr[cur]);
#endif

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

	printf("Testing memory from %08x to %08x", start, end);

	for (addr = start; addr < end; addr += 4) {
		if (! (addr & 0xffff))
			printf("\nAddress %08x...", addr);

		writel(val, addr);
		test = readl(addr);
		if (test != val)
			printf("Address %08x appears to be invalid!  "
				"Expected 0xdeadbeef, got 0x%08x\n",
				addr, test);
	}

	return 0;
}

static int wdt_kick(void)
{
//	writel(0x808, 0xa0030004);
	writel(0x1971, 0xa0030008);
//	writel(readl(0xa0030000) | 0x2201, 0xa0030000);
	return 0;
}

static int wdt_reboot(void)
{
	writel(0x1209, 0xa003001c);
	return 0;
}

static int memory_search(uint32_t start, uint32_t length, uint32_t nonce, uint32_t mask)
{
	int addr;
	register uint32_t test;
	uint32_t end = start + length;

	printf("Looking in memory from %08x to %08x for nonce %08x\n",
			start, end, nonce);

	for (addr = start; addr < end; addr += 0x800) {
	//	if (! (addr & 0xffff)) {
			printf("\rAddress %08x...", addr);
			wdt_kick();
	//	}

		test = readw(addr);
		if ((test & mask) == nonce)
			printf("    %08x matches nonce\n", addr);
	}

	return 0;
}

static int list_registers(void)
{
	int var;

	printf("Registers:\n");

	printf("CPSR: ");
	asm volatile ("mrs %0, cpsr":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("SPSR: ");
	asm volatile ("mrs %0, spsr":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R0:   ");
	asm volatile ("mov %0, r0":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R1:   ");
	asm volatile ("mov %0, r1":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R2:   ");
	asm volatile ("mov %0, r2":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R3:   ");
	asm volatile ("mov %0, r3":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R4:   ");
	asm volatile ("mov %0, r4":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R5:   ");
	asm volatile ("mov %0, r5":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R6:   ");
	asm volatile ("mov %0, r6":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R7:   ");
	asm volatile ("mov %0, r7":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R8:   ");
	asm volatile ("mov %0, r8":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R9:   ");
	asm volatile ("mov %0, r9":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R10:  ");
	asm volatile ("mov %0, r10":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("FP:   ");
	asm volatile ("mov %0, r11":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("IP:   ");
	asm volatile ("mov %0, r12":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("SP:   ");
	asm volatile ("mov %0, r13":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R14:  ");
	asm volatile ("mov %0, r14":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("R15:  ");
	asm volatile ("mov %0, r15":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	printf("LR:   ");
	asm volatile ("mov %0, lr":"=r" (var));
	serial_puth(var, 8);
	printf("\n");

	return 0;
}

static int read_address(uint8_t *arg)
{
	uint32_t addr = _strtoul(arg, 0, 16);
	printf("%08x: %08x\n", addr, readl(addr));
	return 0;
}

static int write_address(uint8_t *arg)
{
	uint8_t *valpos;
	uint32_t val;
	uint32_t addr = _strtoul(arg, (void **)&valpos, 16);
	val = _strtoul(valpos, 0, 16);
	printf("%08x: %08x\n", addr, val);
	writel(val, addr);
	return 0;
}

static int hex_print(uint8_t *arg)
{
	uint8_t *valpos;
	uint32_t *addr = (uint32_t *)_strtoul(arg, (void **)&valpos, 16);
	uint32_t count = 0;
	int i;

	if (valpos)
		count = _strtoul(valpos, 0, 16);
	if (!count)
		count = 256;

	uint32_t data[count / 4];
	for (i = 0; i < ((sizeof(data) / sizeof(*data))); i++)
		data[i] = addr[i];

	printf("%08x:\n", (uint32_t)addr);
	serial_print_hex(data, sizeof(data));
	printf("\n");
	return 0;
}

#define IRQ_BASE 0xa0060180

#define IRQ_MASK_SET_LOW 0x20
#define IRQ_MASK_SET_HIGH 0x24

#define IRQ_MASK_CLEAR_LOW 0x40
#define IRQ_MASK_CLEAR_HIGH 0x44

#define IRQ_SCAN_DEPTH 0x1000
#define IRQ_SCAN_START 0x020
static int find_irqs(void)
{
	/* There should be a mask register, a mask-clear register, and a
	   mask-set register.  Look through memory for a register containing
	   the value 0xffffffff.  Then, try setting it to 0x00000000.  If
	   that fails, try another address.

	   If that succeeds, mark it as a candidate for the IRQ Mask
	   Register.

	   Then, begin writing 0xffffffff to other registers, trying to make
	   the candidate go from 0x00000000 to 0xffffffff.  If no candidate
	   is found, continue.

	   If you find this, then you have a good candidate for the IRQ Mask
	   Set register.  Begin looking again for a register that, when
	   written to, sets the mask register to 0x00000000 again.
	 */

	int base_num;
	uint32_t base = 0xa0050000;

	printf("Trying to look for IRQ tables...\n");

	writel(0, 0xa0180820);
	//for (base = 0xa0180850; base < 0xa0180a00; base += 4) {
	for (base = 0xa0010410; base < 0xa0030500; base += 4) {
		printf("Investigating addr 0x%08x...\n", base);
		writel(1, base);
	}

#if 0
	{
		uint32_t mask_offset;
		uint32_t set_offset;
		uint32_t clear_offset;

		//if (!(base & 0x7ffff))
			printf("Investigating base 0x%08x...\n", base);
		wdt_kick();
		/* Look for mask register */
		for (mask_offset = IRQ_SCAN_START;
			mask_offset < IRQ_SCAN_DEPTH;
			mask_offset += 4) {

			/* In our little world, offset registers can be set
			 * to 0xffffffff */

			/* Try setting the register, make sure it sets */
			writel(0xffffffff, mask_offset + base);
			if (readl(mask_offset + base) == 0)
				continue;

			/* Try clearing the register, make sure it clears */
			writel(0x00000000, mask_offset + base);
			if (readl(mask_offset + base) != 0x00000000)
				continue;

			writel(0x00000000, mask_offset + base);
			if (readl(mask_offset + base) != 0x00000000)
				continue;

			writel(0xffffffff, mask_offset + base);
			if (readl(mask_offset + base) == 0)
				continue;

			writel(0xffffffff, mask_offset + base);
			if (readl(mask_offset + base) == 0)
				continue;

			printf("    Candidate mask register at 0x%08x\n",
					base + mask_offset);

			/* Candidate mask offset.  Look for clear
			 * register */
			wdt_kick();
			for (clear_offset = (mask_offset - 0x20);
				clear_offset < (mask_offset + 0x20);
				clear_offset += 4) {
				uint32_t old_mask;

				if (clear_offset == mask_offset)
					continue;

				writel(0xffffffff, mask_offset + base);
				old_mask = readl(mask_offset + base);
				writel(0xffffffff, clear_offset + base);
				if (readl(mask_offset + base) == old_mask)
					continue;

				printf("    Candidate clear register at 0x%08x\n",
						base + clear_offset);

				/* Candidate for mask and clear offsets,
				 * look for set offset
				 */
				wdt_kick();
				for (set_offset = (mask_offset - 0x20);
					set_offset < (mask_offset + 0x20);
					set_offset += 4) {

					if (set_offset == mask_offset)
						continue;

					if (set_offset == clear_offset)
						continue;

					/* If we write and the mask changes, we
					 * might have a set register */
					writel(0x00000000, mask_offset + base);
					old_mask = readl(mask_offset + base);
					writel(0xffffffff, set_offset + base);
					if (readl(mask_offset + base) == old_mask)
						continue;



	printf("Candidate for IRQ mask/set/clear:\n");
	printf("    Base:  0x%08x\n", base);
	printf("    Mask:  0x%08x\n", mask_offset);
	printf("    Set:   0x%08x\n", set_offset);
	printf("    Clear: 0x%08x\n", clear_offset);
	while(1)
		wdt_kick();

				}
			}
		}
	}
#endif
	return 0;
}

static int enable_irq(void)
{
	int var;
	asm volatile ("mrs %0, cpsr":"=r" (var));
	if (!(var & 0x80)) {
		printf("Interrupts already enabled\n");
		return -1;
	}

	printf("Interrupts were disabled.  Re-enabling...\n");
	var &= ~0x80;
	var &= ~0x1f;
	var |= 0x10;
	asm volatile ("msr cpsr, %0":"=r" (var));

	return 0;
}

static int enable_fiq(void)
{
	int var;
	asm volatile ("mrs %0, cpsr":"=r" (var));
	if (!(var & 0x40)) {
		printf("FIQ already enabled\n");
		return -1;
	}

	printf("FIQ was disabled.  Re-enabling...\n");
	var &= ~0x40;
	asm volatile ("msr cpsr, %0":"=r" (var));

	return 0;
}

static int setup_priorities(void)
{
	writel(0x43415453, 0xf0244a88);
	writel(0x444e454b, 0xf0244a8c);

	writel(0x43415453, 0x7000a5d8);
	writel(0x444e454b, 0x7000a5dc);

	writel(0x43415453, 0xf0244a08);
	writel(0x444e454b, 0xf0244a0c);

	writel(0x43415453, 0xf0244908);
	writel(0x444e454b, 0xf024490c);

	writel(0x43415453, 0xf0244988);
	writel(0x444e454b, 0xf0244988);

	writel(0x43415453, 0xf0244988);
	writel(0x444e454b, 0xf024498c);

	writel(0x43415453, 0x7000a680);
	writel(0x444e454b, 0x7000a684);
	return 0;
}

static int do_init(void)
{
	void *rv_start = (void *)0x10003460;
	void *rv_end = (void *)0x100034e0;
	serial_init();
	setup_priorities();

	/* Kick WDT */
	writel(0x1971, 0xa0030008);

	printf("\n\nFernly shell\n");
	printf("Copying vectors");
	printf("   Src: %p", rv_start);
	printf(" Src end: %p", rv_end);
	printf(" Size: %d\n", rv_end - rv_start);
	printf("(%d bytes from %p to 0)...\n", rv_end - rv_start, rv_start);
	_memcpy((void *)0, rv_start, rv_end - rv_start);
	enable_irq();
	enable_fiq();

	return 0;
}

int main(int argc, char **argv)
{
	do_init();

	while (1) {
		uint8_t line[256];
		printf(PROMPT);
		serial_get_line(line, sizeof(line));
#if !defined(AUTOMATED)
		printf("\n");
#endif

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

			case 'i':
				find_irqs();
				break;

			case 'r':
				read_address(line + 1);
				break;

			case 'w':
				write_address(line + 1);
				break;

			case 'd':
				printf("Kicking WDT...\n");
				wdt_kick();
				break;

			case 'b':
				printf("Rebooting (using WDT)...\n");
				wdt_reboot();
				break;

			case 'c':
				asm volatile ("swi 3");
				break;

			default:
				printf("Unknown command\n");
				break;
		}
	}

	return 0;
}
