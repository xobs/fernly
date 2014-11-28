#include <string.h>
#include "bionic.h"
#include "printf.h"

int cmd_msleep(int argc, char **argv)
{
	uint32_t msecs;

	if (argc != 1) {
		printf("Usage: msleep [milliseconds]\n");
		return 1;
	}

	msecs = strtoul(argv[0], NULL, 0);
	_msleep(msecs);
	return 0;
}

int cmd_usleep(int argc, char **argv)
{
	uint32_t usecs;

	if (argc != 1) {
		printf("Usage: usleep [microseconds]\n");
		return 1;
	}

	usecs = strtoul(argv[0], NULL, 0);
	_usleep(usecs);
	return 0;
}
