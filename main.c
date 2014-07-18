#include <string.h>
#include "serial.h"
#include "utils.h"
#include "bionic.h"

#define AUTOMATED

#if defined(AUTOMATED)
#define PROMPT "fernly> \n"
#else
#define PROMPT "fernly> "
#endif

/* memcpy utils, from extbl */
const char iram_utils_0[] =
	"\x02\x00\x51\xe1\x04\x30\x90\x34\x04\x30\x81\x34\xfb\xff\xff\x3a"
	"\x0e\xf0\xa0\xe1\x01\x00\x50\xe1\xf8\xff\xff\x8a\x0e\xf0\xa0\x01"
	"\x01\x30\x42\xe0\x04\x30\x43\xe2\x01\x20\xa0\xe1\x03\x00\x80\xe0"
	"\x03\x10\x81\xe0\x02\x00\x51\xe1\x04\x30\x10\xa4\x04\x30\x01\xa4"
	"\xfb\xff\xff\xaa\x0e\xf0\xa0\xe1\x00\x20\xa0\xe3\x01\x00\x50\xe1"
	"\x04\x20\x80\x34\xfb\xff"
	;
const int iram_utils_0_size = 86;

const char iram_utils_1[] =
	"\xf8\xb5\x00\x10\xd0\x73\x00\x70\xe8\x00\x00\x00\xb4\x10\x00\x00"
	"\xb8\x74\x00\x70\x28\x92\x00\x10\x00\x50\x00\x70\x74\x23\x00\x00"
	"\x00\x00\x00\x00\x74\x73\x00\x70\x50\x91\x00\x10\x00\x20\x01\x00"
	"\xd8\x00\x00\x00\x8c\x0d\x00\x00\xd8\x20\x01\x00\x9c\xb5\x00\x10"
	"\x74\x73\x00\x70\x5c\x00\x00\x00\x30\xb5\x08\x00\x80\x30\xc2\x6a"
	"\x01\x24\x02\x23\x14\x70\x53\x70\x80\x23\x93\x70\x3c\x25\xd3\x70"
	"\x15\x71\x53\x71\x03\x6b\x9f\x22\x9a\x72\x03\x6b\x35\x22\xda\x73"
	"\x03\x6b\xc0\x31\x5a\x73\x03\x6b\x05\x22\xda\x72\x02\x6b\x14\x73"
	"\x03\x6b\x75\x22\x9a\x70\x00\x6b\x7a\x22\xc2\x70\x16\x20\x08\x70"
	"\x9e\x48\x48\x61\x00\x20\x30\xbd\x00\x20\x70\x47\x30\xb5\x0d\x00"
	"\x80\x31\xcc\x6a\x29\x00\xff\xf7\xcf\xff\x00\x20\xa0\x70\xe0\x70"
	"\x02\x20\xc0\x35\x28\x70\x00\x20\x30\xbd\x02\x00\xc0\x32\x10\xb5"
	"\x00\x29\x02\xd1\x51\x69\x88\x47\x0c\xe0\x01\x29\x0a\xd1\x51\x78"
	"\xc2\x29\x07\xd1\x41\x69\x01\x20\x09\x68\x80\x04\x81\x42\x01\xd1"
	"\xc2\x20\x10\xbd\x00\x20\x10\xbd"
	;

extern void ram_memcpy(const void *src, int dest_start, int dest_end);
extern void ram_bzero(int dest_start, int dest_end);
#define RAM_MEMCPY_OFFSET ((void *)0x70007374)

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

static void writeb(uint8_t value, uint32_t addr)
{
	*((volatile uint8_t *)addr) = value;
}

static uint8_t readb(uint32_t addr)
{
	return *(volatile uint8_t *)addr;
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
	writel(0x1971, 0xa0030008);
	return 0;
}

static int wdt_reboot(void)
{
	writel(0x1209, 0xa003001c);
	return 0;
}

int memory_search(uint32_t start, uint32_t length, uint32_t nonce, uint32_t mask)
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
	int bytes = 4;
	uint32_t addr;

	/* If the address begins with 'o', read one byte, 't' two, 'f' four */
	if (*arg == 't') {
		bytes = 2;
		arg++;
	}
	else if (*arg == 'o') {
		bytes = 1;
		arg++;
	}
	else if (*arg == 'f') {
		bytes = 4;
		arg++;
	}

	addr = _strtoul(arg, 0, 16);
  
	if (bytes == 1)
		printf("%08x: %02x\n", addr, readb(addr));
	else if (bytes == 2)
		printf("%08x: %04x\n", addr, readw(addr));
	else
		printf("%08x: %08x\n", addr, readl(addr));

	return 0;
}

static int write_address(uint8_t *arg)
{
	uint8_t *valpos;
	uint32_t val;
	uint32_t addr;
	int bytes = 4;

	/* If the address begins with 'o', read one byte, 't' two, 'f' four */
	if (*arg == 't') {
		bytes = 2;
		arg++;
	}
	else if (*arg == 'o') {
		bytes = 1;
		arg++;
	}
	else if (*arg == 'f') {
		bytes = 4;
		arg++;
	}

	addr = _strtoul(arg, (void **)&valpos, 16);
	val = _strtoul(valpos, 0, 16);

	if (bytes == 1) {
		printf("%08x: %02x\n", addr, val);
		writeb(val, addr);
	}
	else if (bytes == 2) {
		printf("%08x: %04x\n", addr, val);
		writew(val, addr);
	}
	else if (bytes == 4) {
		printf("%08x: %08x\n", addr, val);
		writel(val, addr);
	}

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

	//int base_num;
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

static void gpio_set(unsigned int gpio, int value)
{
	uint32_t gpio_dr_reg;
	uint32_t gpio_dr_mask;
	uint32_t gpio_dr_val;

	uint32_t gpio_dout_reg;
	uint32_t gpio_dout_mask;
	uint32_t gpio_dout_val;

	uint32_t val;

	if (gpio < 16) {
		gpio_dr_reg   = 0xa0020000;
		gpio_dout_reg = 0xa0020300;
	}
	else if (gpio < 32) {
		gpio_dr_reg   = 0xa0020010;
		gpio_dout_reg = 0xa0020310;
	}
	else if (gpio < 48) {
		gpio_dr_reg   = 0xa0020020;
		gpio_dout_reg = 0xa0020320;
	}
	else if (gpio < 64) {
		gpio_dr_reg   = 0xa0020030;
		gpio_dout_reg = 0xa0020330;
	}
	else if (gpio < 73) {
		gpio_dr_reg   = 0xa0020040;
		gpio_dout_reg = 0xa0020340;
	}
	else {
		printf("Invalid GPIO selected: %d\n", gpio);
		return;
	}

	gpio_dr_mask = ~(1 << (gpio & 15));
	gpio_dout_mask = ~(1 << (gpio & 15));

	/* Values less than 0 indicate input */
	if (value < 0) {
		gpio_dr_val = 0;
		gpio_dout_val = 0; /* Value doesn't matter */
	}
	else {
		gpio_dr_val = 1 << (gpio & 15);
		if (value > 0)
			gpio_dout_val = 1 << (gpio & 15);
		else
			gpio_dout_val = 0;
	}

//	printf("Setting GPIO%d -> %d\n", gpio, value);

	val = (readw(gpio_dout_reg) & gpio_dout_mask) | gpio_dout_val;
//	printf("	DOUT 0x%04x: 0x%04x -> 0x%04x\n",
//		gpio_dout_reg, readw(gpio_dout_reg), val);
	writew(val, gpio_dout_reg);

	val = (readw(gpio_dr_reg) & gpio_dr_mask) | gpio_dr_val;
//	printf("	DR 0x%04x: 0x%04x -> 0x%04x\n",
//		gpio_dr_reg, readw(gpio_dr_reg), val);
	writew(val, gpio_dr_reg);
}

static const uint8_t gpio_step_vals[] = {
	50, 27, 26, 10, 3, 4, 25,
};

/* Connector pinout:
   1 -
   2 -
   3 - GPIO27 / MCCM0 / JTDO
   4 - (pwr?)
   5 - GPIO25 / MCCK / JTRCK
   6 - GND
   7 - GPIO26 / MCDA0 / JTRSTB
   8 -
*/
static int step_gpio(int step)
{
	gpio_set(gpio_step_vals[step], 0);
	step++;
	if (step > 6)
		step = 0;

	if (step == 0)
		printf("Step %d GPIO%d - MC2CM (JRTCK)\n",
			step, gpio_step_vals[step]);
	else if (step == 1) // Pin 3
		printf("Step %d GPIO%d - MCCM0 (JTDO), pin 3\n",
			step, gpio_step_vals[step]);
	else if (step == 2) // Pin 7
		printf("Step %d GPIO%d - MCDA0 (JTRSTB)\n",
			step, gpio_step_vals[step]);
	else if (step == 3)
		printf("Step %d GPIO%d - MCDA3 (JTCK)\n",
			step, gpio_step_vals[step]);
	else if (step == 4)
		printf("Step %d GPIO%d - MCDA1 (JTDI)\n",
			step, gpio_step_vals[step]);
	else if (step == 5)
		printf("Step %d GPIO%d - MCDA2 (NONE)\n",
			step, gpio_step_vals[step]);
	else if (step == 6)
		printf("Step %d GPIO%d - MCCK (JTRCK)\n",
			step, gpio_step_vals[step]);

	gpio_set(gpio_step_vals[step], 1);

	return step;
}

#define set_gpio_mode(addr, offset, value) \
	writel((readl(addr) & ~(0xf << offset)) | (value << offset), addr)
static int enable_jtag(void)
{
	int mode;

	mode = 0;

	if (mode == 0) {
		printf("Setting up JTAG using SD connector\n");

		printf("\tGPIO50 (MC2CM) -> JRTCK\n");
		set_gpio_mode(0xa0020c60, 8, 5);

		printf("\tGPIO27 (MCCM0) -> JTDO\n");
		set_gpio_mode(0xa0020c30, 12, 5);

		printf("\tGPIO26 (MCDA0) -> JTRSTB\n");
		set_gpio_mode(0xa0020c30, 8, 5);

		printf("\tGPIO25 (MCCK) -> JTRCK\n");
		set_gpio_mode(0xa0020c30, 4, 5);

		printf("\tGPIO10 (MCDA3) -> JTCK\n");
		set_gpio_mode(0xa0020c10, 8, 5);

		printf("\tGPIO3  (MCDA1) -> JTDI\n");
		set_gpio_mode(0xa0020c00, 12, 5);
	}
	else {
		printf("Setting up JTAG using camera module\n");

		printf("\tGPIO48 CMDAT1 -> JTMS\n");
		printf("\tGPIO49 CMDAT2 -> JTCK\n");
		printf("\tGPIO50 CMDAT3 -> JRTCK\n");
		printf("\tGPIO51 CMDAT4 -> JTRST_B\n");
		printf("\tGPIO52 CMDAT5 -> JTDO\n");

		writel(0x00055555, 0xa0020c60);
	}

	printf("\tDone.\n");
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
	int i;

	serial_init();
	//setup_priorities();

	/* Kick WDT */
	writel(0x1971, 0xa0030008);

	/* Disable WDT */
	writel(0x2200, 0xa0030000);

	printf("\n\nFernly shell\n");

	/* Copy some utils to iram */
	printf("Installing iram utils memcpy");
	_memcpy(RAM_MEMCPY_OFFSET, iram_utils_0, iram_utils_0_size);

	printf(" ram_memcpy(0)");
	ram_memcpy(0, 0, 0);

	printf(" ram_bzero(0)");
	ram_bzero(0, 0);

	printf(" ram_memcpy(0x700073d0)");
	ram_memcpy(iram_utils_1, 0x700073d0, 0x700074b8);

	printf(" ram_bzero(0x700074b8)");
	ram_bzero(0x700074b8, 0x7000856c);

	printf(" ok.\n");

	/* Copy exception vectors to address 0 */
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

int main(void)
{
	int step = 0;
	list_registers();
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

			case 'h':
				hex_print(line + 1);
				break;

			case 'l':
				list_registers();
				break;

			case 'i':
				find_irqs();
				break;

			case 'j':
				enable_jtag();
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

			case 's':
				step = step_gpio(step);
				break;

			default:
				printf("Unknown command\n");
				break;
		}
	}

	return 0;
}
