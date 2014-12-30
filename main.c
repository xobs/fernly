#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"
#include "utils.h"
#include "scriptic.h"

#include "fernvale-pmic.h"

//#define AUTOMATED

#if !defined(AUTOMATED)
#define PROMPT "fernly> "

static int serial_get_line(char *bfr, int len)
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
		       (void)next;

                       next = serial_getc();
               }
               else
                       cur++;
       }
       bfr[len - 1] = '\0';
       return -1;
}
#endif

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

	serial_puts("LR:   ");
	asm volatile ("mov %0, r14":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	serial_puts("PC:   ");
	asm volatile ("mov %0, r15":"=r" (var));
	serial_puth(var, 8);
	serial_puts("\n");

	return 0;
}

static int shell_run_command(char *line);
static int do_init(void)
{
	serial_init();

	list_registers();

	/* Disable system watchdog */
	writel(0x2200, 0xa0030000);

	/* Enable USB Download mode (required for no-battery operation) */
	writew(0x8000, PMIC_CTRL10);

	/* Disable battery watchdog */
	writew(0x2, PMIC_CTRL9);

	scriptic_run("set_plls");
	scriptic_run("enable_psram");

	serial_puts("\n\nFernly shell\n");
	shell_run_command("bl 5");
	shell_run_command("lcd init");
	shell_run_command("lcd tpd");

	return 0;
}

#ifdef AUTOMATED
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

/* Protocol:
 * Stream is byte-oriented.  The following commands are known:
 *
 * r - read an address
 * w - write to an address
 * z - zero-fill a region
 *
 * Responses:
 *
 * k - Ready for command
 * ? - Unknown command
 */
static int loop(void)
{
	int buf;
	int size;
	uint32_t offset;
	uint32_t value;

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

	case 'f':
		size = get_hex(4);
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
	return 0;
}
#else /* AUTOMATED */

static int cmd_help(int argc, char **argv);
extern int cmd_hex(int argc, char **argv);
extern int cmd_irq(int argc, char **argv);
extern int cmd_msleep(int argc, char **argv);
extern int cmd_peek(int argc, char **argv);
extern int cmd_poke(int argc, char **argv);
extern int cmd_spi(int argc, char **argv);
extern int cmd_spi_raw(int argc, char **argv);
extern int cmd_swi(int argc, char **argv);
extern int cmd_reboot(int argc, char **argv);
extern int cmd_led(int argc, char **argv);
extern int cmd_bl(int argc, char **argv);
extern int cmd_lcd(int argc, char **argv);
extern int cmd_load(int argc, char **argv);
extern int cmd_loadjump(int argc, char **argv);

static const struct {
	int (*func)(int argc, char **argv);
	const char *name;
	const char *help;
} commands[] = {
	{
		.func = cmd_help,
		.name = "help",
		.help = "Print help about available commands",
	},
	{
		.func = cmd_reboot,
		.name = "reboot",
		.help = "Reboot Fernvale",
	},
	{
		.func = cmd_msleep,
		.name = "msleep",
		.help = "Sleep for some number of milliseconds",
	},
	{
		.func = cmd_hex,
		.name = "hex",
		.help = "Print area of memory as hex",
	},
	{
		.func = cmd_peek,
		.name = "peek",
		.help = "Look at one area of memory",
	},
	{
		.func = cmd_poke,
		.name = "poke",
		.help = "Write a value to an area of memory",
	},
	{
		.func = cmd_irq,
		.name = "irq",
		.help = "Manipulate IRQs",
	},
	{
		.func = cmd_spi,
		.name = "spi",
		.help = "Manipulate on-board SPI",
	},
	{
		.func = cmd_spi_raw,
		.name = "spi_raw",
		.help = "Manipulate on-board SPI (raw interface)",
	},
	{
		.func = cmd_swi,
		.name = "swi",
		.help = "Generate software interrupt",
	},
	{
		.func = cmd_led,
		.name = "led",
		.help = "Turn the on-board LED on or off",
	},
	{
		.func = cmd_bl,
		.name = "bl",
		.help = "Set the LCD backlight brightness",
	},
	{
		.func = cmd_lcd,
		.name = "lcd",
		.help = "Manipulate the LCD",
	},
	{
		.func = cmd_load,
		.name = "load",
		.help = "Load data to a specific area in memory",
	},
	{
		.func = cmd_loadjump,
		.name = "loadjmp",
		.help = "Load data to a specific area in memory, "
			"then jump to it",
	},
};

int cmd_help(int argc, char **argv)
{
	int i;

	printf("Fernly shell help.  Available commands:\n");
	for (i = 0; i < sizeof(commands) / sizeof(*commands); i++) {
		serial_putc('\t');
		serial_puts(commands[i].name);
		serial_putc('\t');
		serial_puts(commands[i].help);
		serial_putc('\n');
	}
	return 0;
}

static int shell_run_command(char *line)
{
	char *lp, *cmd, *tokp;
	char *args[8];
	int i, n;

	lp = _strtok(line, " \t", &tokp);
	cmd = lp;
	n = 0;
	while ((lp = _strtok(NULL, " \t", &tokp)) != NULL) {
		if (n >= 7) {
			printf("too many arguments\r\n");
			cmd = NULL;
			break;
		}
		args[n++] = lp;
	}
	args[n] = NULL;
	if (cmd == NULL)
		return -1;

	for (i = 0; i < sizeof(commands) / sizeof(*commands); i++)
		if (!_strcasecmp(commands[i].name, cmd))
			return commands[i].func(n, args);

	printf("Unknown command: %s\n", cmd);

	return 0;
}

static int loop(void)
{
	char line[256];

	serial_puts(PROMPT);
	serial_get_line(line, sizeof(line));
	printf("\n");
	return shell_run_command(line);
}
#endif /* ! AUTOMATED */

int main(void)
{
	do_init();

	while (1)
		loop();

	return 0;
}
