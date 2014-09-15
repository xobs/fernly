#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"
#include "fernvale-lcd.h"

// local implementation of strncmp() because we don't have it as a lib in fernly
static int my_strncmp(const char* s1, const char* s2, size_t n) 
{
  while(n--) {
    if( *s2 == '\0' )  // abort checking if we reach the end of s2
      return 0;
    if(*s1++!=*s2++)
      return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
  }
  return 0;
}

int cmd_lcd(int argc, char **argv)
{
	uint32_t genarg = 0;
	char *subcmd;

	if (argc < 1) {
	  printf("Usage: lcd [subcmd] [arg0]\n");
		return -1;
	}

	subcmd = argv[0];

	if (argc == 2 ) {
	  genarg = _strtoul(argv[1], NULL, 0);
	}

	printf( "command got: %s\n", subcmd );
	if( my_strncmp( subcmd, "su", 8 ) == 0 ) {
	  // execute setup command
	  printf( "nothing for now.\n" );
	} else if( my_strncmp( subcmd, "dump", 8 ) == 0 ) {
	  // dump registers
	  printf( "LCD_PAR0_CMD_PORT_REG: %08x\n", *((volatile uint32_t *) LCD_PAR0_CMD_PORT_REG) );
	  printf( "LCD_PAR0_DAT_PORT_REG: %08x\n", *((volatile uint32_t *) LCD_PAR0_DAT_PORT_REG) );
	  printf( "LCD_PAR1_CMD_PORT_REG: %08x\n", *((volatile uint32_t *) LCD_PAR1_CMD_PORT_REG) );
	  printf( "LCD_PAR1_DAT_PORT_REG: %08x\n", *((volatile uint32_t *) LCD_PAR1_DAT_PORT_REG) );
	  printf( "LCD_PAR0_CFG_REG: %08x\n", *((volatile uint32_t *) LCD_PAR0_CFG_REG) );
	  printf( "LCD_PAR1_CFG_REG: %08x\n", *((volatile uint32_t *) LCD_PAR1_CFG_REG) );
	  printf( "LCD_STATUS_REG: %08x\n", *((volatile uint32_t *) LCD_STATUS_REG) );
	  printf( "LCD_INT_ENA_REG: %08x\n", *((volatile uint32_t *) LCD_INT_ENA_REG) );
	  printf( "LCD_INT_STAT_REG: %08x\n", *((volatile uint32_t *) LCD_INT_STAT_REG) );
	  printf( "LCD_RUN_REG: %08x\n", *((volatile uint32_t *) LCD_RUN_REG) );
	  printf( "LCD_RESET_REG: %08x\n", *((volatile uint32_t *) LCD_RESET_REG) );
	  printf( "LCD_PAR_DATA_WIDTH_REG: %08x\n", *((volatile uint32_t *) LCD_PAR_DATA_WIDTH_REG) );
	  printf( "LCD_TEARING_CON_REG: %08x\n", *((volatile uint32_t *) LCD_TEARING_CON_REG) );
	} else {
	  printf( "lcd commands: su (setup timing), dump (dump registers)\n" );
	}

	return 0;
}

