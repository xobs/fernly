#include <string.h>
#include "bionic.h"
#include "serial.h"
#include "printf.h"
#include "memio.h"

int cmd_load(int argc, char **argv)
{
	uint32_t offset;
	uint32_t total;
	uint32_t left;

	if (argc != 2) {
		printf("Usage: load [offset] [size]\n"
			"Loads [size] bytes to address [offset]\n");
		return 1;
	}

	offset = strtoul(argv[0], NULL, 0);
	total = strtoul(argv[1], NULL, 0);

	left = total;
	while (left--)
		writeb(serial_getc(), offset++);

	return 0;
}

int cmd_loadjump(int argc, char **argv)
{
	uint32_t offset;
	uint32_t total;
	uint32_t left;

	void (*jumpaddr)(void);

	if (argc != 2) {
		printf("Usage: loadjmp [offset] [size]\n"
			"Loads [size] bytes to address [offset], and "
			"jumps to [offset] afterwards.\n");
		return 1;
	}

	offset = strtoul(argv[0], NULL, 0);
	total = strtoul(argv[1], NULL, 0);
	jumpaddr = (void (*)(void))offset;

	left = total;

	while (left--)
		writeb(serial_getc(), offset++);

	jumpaddr();

	return 0;
}
