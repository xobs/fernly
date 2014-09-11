#include <string.h>

#include "bionic.h"
#include "irq.h"
#include "printf.h"

static void print_help(void)
{
	printf("Usage:\n");
	printf("irq sim [num]      Simulate IRQ [num]\n");
	printf("irq enable [num]   Enable IRQ [num]\n");
	printf("irq disable [num]  Disable IRQ [num]\n");
}

int cmd_irq(int argc, char **argv)
{
	int num;

	if (argc != 2) {
		print_help();
		return -1;
	}

	num = _strtoul(argv[1], NULL, 0);
	if (num >= __irq_max__) {
		printf("Only %d IRQs present\n", __irq_max__);
		return -1;
	}

	if (!_strcasecmp(argv[0], "sim")) {
		printf("Simulating IRQ %d\n", num);
		irq_stimulate(num);
	}

	else if (!_strcasecmp(argv[0], "enable")) {
		printf("Enabling IRQ %d\n", num);
		irq_enable(num);
	}

	else if (!_strcasecmp(argv[0], "disable")) {
		printf("Disabling IRQ %d\n", num);
		irq_disable(num);
	}

	else {
		printf("Unknown command\n");
		print_help();
		return -1;
	}

	return 0;
}

int cmd_swi(int argc, char **argv)
{
	printf("Generating SWI...\n");
	asm volatile ("swi #0\n");
	printf("Returned from SWI\n");

	return 0;
}
