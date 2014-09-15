#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"
#include "fernvale-kbd.h"

int cmd_led(int argc, char **argv)
{
	uint32_t state;

	if (argc < 1) {
		printf("Usage: led [1 = on, 0 = off]\n");
		return -1;
	}

	state = _strtoul(argv[0], NULL, 0);

	if( state ) {
	  *((volatile uint32_t *) BIG_LED_ADDR) = BIG_LED_ON;
	} else {
	  *((volatile uint32_t *) BIG_LED_ADDR) = BIG_LED_OFF;
	}

	return 0;
}

