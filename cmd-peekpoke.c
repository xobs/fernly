#include <string.h>
#include <inttypes.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"

int cmd_peek(int argc, char **argv)
{
	uint32_t offset;

	if (argc < 1) {
		printf("Usage: peek [offset]\n");
		return -1;
	}

	offset = strtoul(argv[0], NULL, 0);

	printf("Value at 0x%08"PRIx32": ", offset);
	printf("0x%08"PRIx32"\n", *((volatile uint32_t *)offset));
	return 0;
}

int cmd_poke(int argc, char **argv)
{
	uint32_t offset;
	uint32_t val;

	if (argc < 2) {
		printf("Usage: poke [offset] [val]\n");
		return -1;
	}

	offset = strtoul(argv[0], NULL, 0);
	val = strtoul(argv[1], NULL, 0);

	printf("Setting value at 0x%08"PRIx32" to 0x%08"PRIx32": ",
		offset, val);
	writel(val, offset);
	printf("Ok\n");

	return 0;
}
