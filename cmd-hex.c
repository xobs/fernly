#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"
#include "utils.h"

int cmd_hex(int argc, char **argv)
{
	uint32_t offset;
	int count = 0x200;
	if (argc < 1) {
		printf("Usage: hex [offset] [[count]]\n");
		return -1;
	}

	offset = strtoul(argv[0], NULL, 0);

	if (argc > 1)
		count = strtoul(argv[1], NULL, 0);

	serial_print_hex((const void *)offset, count);
	return 0;
}
