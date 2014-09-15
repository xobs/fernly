#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"
#include "fernvale-bl.h"

int cmd_bl(int argc, char **argv)
{
	uint32_t level;
	int i;

	if (argc < 1) {
	  printf("Usage: bl [level 0-%d]\n", BLLED_MAX_LEVEL);
		return -1;
	}

	level = _strtoul(argv[0], NULL, 0);

	if( level > BLLED_MAX_LEVEL ) {
	  level = 0;
	  printf( "Backlight level should be 0-5\n" );
	}

	if( level > 0 ) {
	  for( i = 0; i < BLLED_BANKS; i++ ) {
	    *((volatile uint32_t *) BLLED_REG0_BANK(i)) = 
	      ((level << BLLED_REG0_STEP_BIT) & BLLED_REG0_STEP_MSK) |
	      ((1 << BLLED_REG0_MODE_BIT) | (1 << BLLED_REG0_EN_BIT));
	  }

	} else {
	  for( i = 0; i < BLLED_BANKS; i++ ) {
	    *((volatile uint32_t *) BLLED_REG0_BANK(i)) = 0;  // meh
	  }
	}

	return 0;
}

