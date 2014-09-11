#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"

int cmd_msleep(int argc, char **argv)
{
	uint32_t msecs, i, j;

	if (argc != 1) {
		printf("Usage: msleep [milliseconds]\n");
		return 1;
	}

	msecs = _strtoul(argv[0], NULL, 0);

	for (i = 0; i < msecs; i++) {
		for (j = 0; j < 73000; j++) {
			asm("nop");
		}
	}

	return 0;
}

int cmd_usleep(int argc, char **argv)
{
	uint32_t usecs, i, j;

	if (argc != 1) {
		printf("Usage: usleep [microseconds]\n");
		return 1;
	}

	usecs = _strtoul(argv[0], NULL, 0);

	for (i = 0; i < usecs; i++) {
		for (j = 0; j < 73; j++) {
			asm("nop");
		}
	}

	return 0;
}
