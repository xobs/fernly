#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"
#include "fernvale-lcd.h"
#include "fernvale-gpio.h"
#include "fernvale-clockgate.h"

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

static void usleep(uint32_t usecs) {
  uint32_t i, j;
  for (i = 0; i < usecs; i++) {
    for (j = 0; j < 73; j++) {
      asm("nop");
    }
  }
}

static void msleep(uint32_t msecs) {
  uint32_t i, j;
  for (i = 0; i < msecs; i++) {
    for (j = 0; j < 73000; j++) {
      asm("nop");
    }
  }
}

#define USE_DMA 0
#if USE_DMA  // needs debuggin'
void flush_dma( uint8_t q ) {
  while(*((volatile uint16_t *) LCD_STATUS_REG) & LCD_STATUS_BUSY_BIT)
    ;
  
  *((volatile uint16_t *) LCD_INT_ENA_REG) &= ~LCD_INT_ENA_TRIG_BIT;
  *((volatile uint32_t *) LCD_MEMMAP_CTRL_REG) &= 0x0FFFFFFF; // disable all layers
  *((volatile uint32_t *) LCD_MEMMAP_CTRL_REG) &= 0xFFFFE0FF; // clear command count
  *((volatile uint32_t *) LCD_MEMMAP_CTRL_REG) |= (q << 8); // enter command count
  *((volatile uint32_t *) LCD_MEMMAP_CTRL_REG) |= 0x8000; // enable commands to be sent first
  *((volatile uint32_t *)LCD_MEMMAP_SIZE_REG) = 0x0;
  *((volatile uint16_t *)LCD_RUN_REG) = 0;
  *((volatile uint16_t *)LCD_RUN_REG) = LCD_RUN_BIT;
  usleep(1);

  while(*((volatile uint16_t *) LCD_STATUS_REG) & LCD_STATUS_BUSY_BIT)
    ;

  *((volatile uint32_t *)LCD_MEMMAP_SIZE_REG) = 0x014000F0;
}

void lcd_cmd( uint16_t cmd ) {
  *((volatile uint32_t *)LCD_CMD_LIST_ADDR) = 0x800000 | cmd;
  flush_dma(1);
}

void lcd_dat( uint16_t dat ) {
  *((volatile uint32_t *)LCD_CMD_LIST_ADDR) = dat;
  flush_dma(1);
}

// note these don't flush the DMA, you have to do this yourself explicitly later on
void lcd_cmd_slot( uint16_t cmd, uint8_t slot ) { // slot is the order to execute
  *((volatile uint32_t *) (LCD_CMD_LIST_ADDR + (slot << 2))) = 0x800000 | cmd;
}
void lcd_dat_slot( uint16_t dat, uint8_t slot ) { // slot is the order to execute
  *((volatile uint32_t *) (LCD_CMD_LIST_ADDR + (slot << 2))) = dat;
}
#else

#define lcd_cmd(_cmd_)   *((volatile uint16_t *)LCD_PAR0_CMD_PORT_REG) = (uint16_t) _cmd_
#define lcd_dat(_dat_)   *((volatile uint16_t *)LCD_PAR0_DAT_PORT_REG) = (uint16_t) _dat_

#endif


void setup_lcd_gpio() {
  // LPCE0, LPTE0, LPRSTB
  *((volatile uint32_t *) GPIO_CTRL_MODE5) &= ~( GPIO_CTRL_MODE5_IO40_MASK |
						 GPIO_CTRL_MODE5_IO46_MASK |
						 GPIO_CTRL_MODE5_IO45_MASK );
  *((volatile uint32_t *) GPIO_CTRL_MODE5) |=  ( GPIO_CTRL_MODE5_IO40_LPCE0B |
						 GPIO_CTRL_MODE5_IO45_LPTE0 |
						 GPIO_CTRL_MODE5_IO46_LPRSTB );

  // NLD0-4, LWRB, LRDB, LPA0
  *((volatile uint32_t *) GPIO_CTRL_MODE4) &= ~( GPIO_CTRL_MODE4_IO32_MASK |
						 GPIO_CTRL_MODE4_IO33_MASK |
						 GPIO_CTRL_MODE4_IO34_MASK |
						 GPIO_CTRL_MODE4_IO35_MASK |
						 GPIO_CTRL_MODE4_IO36_MASK |
						 GPIO_CTRL_MODE4_IO37_MASK |
						 GPIO_CTRL_MODE4_IO38_MASK |
						 GPIO_CTRL_MODE4_IO39_MASK );
  *((volatile uint32_t *) GPIO_CTRL_MODE4) |=  ( GPIO_CTRL_MODE4_IO32_NLD4 |
						 GPIO_CTRL_MODE4_IO33_NLD3 |
						 GPIO_CTRL_MODE4_IO34_NLD2 |
						 GPIO_CTRL_MODE4_IO35_NLD1 |
						 GPIO_CTRL_MODE4_IO36_NLD0 |
						 GPIO_CTRL_MODE4_IO37_LWRB |
						 GPIO_CTRL_MODE4_IO38_LRDB |
						 GPIO_CTRL_MODE4_IO39_LPA0 );

  // NLD5-8
  *((volatile uint32_t *) GPIO_CTRL_MODE3) &= ~( GPIO_CTRL_MODE3_IO28_MASK |
						 GPIO_CTRL_MODE3_IO29_MASK |
						 GPIO_CTRL_MODE3_IO30_MASK |
						 GPIO_CTRL_MODE3_IO31_MASK );
  *((volatile uint32_t *) GPIO_CTRL_MODE3) |=  ( GPIO_CTRL_MODE3_IO28_NLD8 |
						 GPIO_CTRL_MODE3_IO29_NLD7 |
						 GPIO_CTRL_MODE3_IO30_NLD6 |
						 GPIO_CTRL_MODE3_IO31_NLD5 );
  
}

int cmd_lcd(int argc, char **argv)
{
	uint32_t genarg = 0;
	char *subcmd;
	int i;

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
	  setup_lcd_gpio();
	  
	  *((volatile uint32_t *) CLKGATE_SYS_CTL0_CLR) = CLKGATE_CTL0_LCD; // power up the LCD block
	  msleep(1);

	  // execute setup command
	  // we're on CS0
	  // our internal bus period is 166 MHz, or 6.25ns
	  // write cycle = 66ns = 11 cycles - 1 = 10
	  // write c22write su (tcs) = 15ns = 3 cycles
	  // write ce2write hold (tdht) = 10ns = 2 cycles - 1 = 1 
	  // read latency = 450 ns = 72 cycles, crop at 63 cycles
	  // read ce2read su (trdl - trcs) = 45-45 = 0 ns = 0 cycles
	  // read th = 90ns = 15 cycles (not quite clear, but best guess)
	  *((volatile uint32_t *)LCD_PAR0_CFG_REG) = 
	    (10 << LCD_PAR_CFG_WR_WAIT_CYC_BIT) | 
	    (3  << LCD_PAR_CFG_WR_TSU_BIT) |
	    (1  << LCD_PAR_CFG_WR_TH_BIT) |
	    (63 << LCD_PAR_CFG_RD_LATENCY_CYC_BIT ) |   // this might need to be shorter??
	    (0 << LCD_PAR_CFG_RD_TSU_BIT) |
	    (15 << LCD_PAR_CFG_RD_TH_BIT);
	  
	  // 9 bit width, tchw is 0 for this chipset (back2back writes allowed)
	  *((volatile uint32_t *)LCD_PAR_DATA_WIDTH_REG) = 
	    (0 << LCD_PAR_W2W_WAIT0_BIT) |
	    (LCD_PAR_BUS_WIDTH_9BIT << LCD_PAR_BUS_WIDTH0_BIT);
	  
	  *((volatile uint32_t *)LCD_AUTOCOPY_CTRL_REG) = 0; // AUTOCOPY off initially

	} else if( my_strncmp( subcmd, "auto", 8 ) == 0 ) {
	  *((volatile uint32_t *)LCD_LAYER0_CTRL_REG) = 0;
	  *((volatile uint32_t *)LCD_LAYER0_OFFSET_REG) = 0;
	  *((volatile uint32_t *)LCD_LAYER0_BUFF_ADDR_REG) = 0;
	  *((volatile uint32_t *)LCD_LAYER0_SIZE_REG) = 0x014000f0;
	  *((volatile uint32_t *)LCD_LAYER0_MEM_OFFSET_REG) = 0x0;
	  *((volatile uint32_t *)LCD_LAYER0_MEM_PITCH_REG) = 0x1e0;

	  *((volatile uint32_t *)LCD_AUTOCOPY_OFFSET_REG) = 0;
	  *((volatile uint16_t *)LCD_AUTOCOPY_CMD_ADDR_REG) = 0x4000;
	  *((volatile uint16_t *)LCD_AUTOCOPY_DATA_ADDR_REG) = 0x4100;
	  *((volatile uint32_t *)LCD_AUTOCOPY_SIZE_REG) = 0x014000F0;
	  *((volatile uint32_t *)LCD_AUTOCOPY_BG_COLOR_REG) = 0x80008000;

	  *((volatile uint32_t *)LCD_AUTOCOPY_CTRL_REG) = 0x85020094;
	  
	} else if( my_strncmp( subcmd, "dump", 8 ) == 0 ) {
	  // dump registers
	  printf( "LCD_PAR0_CMD_PORT_REG: %04x\n", *((volatile uint16_t *) LCD_PAR0_CMD_PORT_REG) );
	  printf( "LCD_PAR0_DAT_PORT_REG: %04x\n", *((volatile uint16_t *) LCD_PAR0_DAT_PORT_REG) );
	  printf( "LCD_PAR1_CMD_PORT_REG: %04x\n", *((volatile uint16_t *) LCD_PAR1_CMD_PORT_REG) );
	  printf( "LCD_PAR1_DAT_PORT_REG: %04x\n", *((volatile uint16_t *) LCD_PAR1_DAT_PORT_REG) );
	  printf( "LCD_PAR0_CFG_REG: %08x\n", *((volatile uint32_t *) LCD_PAR0_CFG_REG) );
	  printf( "LCD_PAR1_CFG_REG: %08x\n", *((volatile uint32_t *) LCD_PAR1_CFG_REG) );
	  printf( "LCD_STATUS_REG: %04x\n", *((volatile uint16_t *) LCD_STATUS_REG) );
	  printf( "LCD_INT_ENA_REG: %04x\n", *((volatile uint16_t *) LCD_INT_ENA_REG) );
	  printf( "LCD_INT_STAT_REG: %04x\n", *((volatile uint16_t *) LCD_INT_STAT_REG) );
	  printf( "LCD_RUN_REG: %04x\n", *((volatile uint16_t *) LCD_RUN_REG) );
	  printf( "LCD_RESET_REG: %04x\n", *((volatile uint16_t *) LCD_RESET_REG) );
	  printf( "LCD_PAR_DATA_WIDTH_REG: %08x\n", *((volatile uint32_t *) LCD_PAR_DATA_WIDTH_REG) );
	  printf( "LCD_TEARING_CON_REG: %08x\n", *((volatile uint32_t *) LCD_TEARING_CON_REG) );
	  printf( "LCD_AUTOCOPY_CTRL_REG: %08x\n", *((volatile uint32_t *) LCD_AUTOCOPY_CTRL_REG) );
	} else if( my_strncmp( subcmd, "run", 8 ) == 0 ) {
	  // cause the interface to run
	  *((volatile uint16_t *)LCD_RUN_REG) = 0;
	  *((volatile uint16_t *)LCD_RUN_REG) = LCD_RUN_BIT;
	} else if( my_strncmp( subcmd, "stop", 8 ) == 0 ) {
	  // cause the interface to stop
	  *((volatile uint16_t *)LCD_RUN_REG) = 0;
	} else if( my_strncmp( subcmd, "init", 8 ) == 0 ) {
	  *((volatile uint16_t *)LCD_RESET_REG) = 1; 
	  usleep(20000);
	  *((volatile uint16_t *)LCD_RESET_REG) = 0;  // turn on reset
	  msleep(20);
	  *((volatile uint16_t *)LCD_RESET_REG) = 1;  // turn off reset
	  msleep(150);

	  lcd_cmd(0x11); //Exit Sleep
	  msleep(50); // Delay 50ms
	  lcd_cmd(0xC0); //Power control
	  lcd_dat(0x26);
	  lcd_cmd(0xC1); //Power control
	  lcd_dat(0x11); //SAP[2:0];BT[3:0]
	  lcd_cmd(0xC5); //VCM control
	  lcd_dat(0x35);
	  lcd_dat(0x3E);
	  lcd_cmd(0xc7);
	  lcd_dat(0xbe);

	  lcd_cmd(0x36); // Memory Access Control
	  lcd_dat(0x48);

	  lcd_cmd(0x3a); // pixel format set
	  lcd_dat(0x55); // 16bpp
	  
	  lcd_cmd(0xB1); // Frame Rate Control
	  lcd_dat(0x00);
	  lcd_dat(0x1b);

	  //--------------ddram ---------------------
	  lcd_cmd(0x2a); // column set
	  lcd_dat(0x00);
	  lcd_dat(0x00);
	  lcd_dat(0x00);
	  lcd_dat(0xEF);
	  lcd_cmd(0x2b); // page address set
	  lcd_dat(0x00);
	  lcd_dat(0x00);
	  lcd_dat(0x01);
	  lcd_dat(0x3F);
	  lcd_cmd(0x34); // tearing effect off
	  //lcd_cmd(0x35); // tearing effect on
	  //lcd_cmd(0xb4); // display inversion
	  //lcd_dat(0x00,0x00);
	  lcd_cmd(0xb7); //entry mode set
	  lcd_dat(0x07);
	  //-----------------display---------------------
	  lcd_cmd(0xb6); // display function control
	  lcd_dat(0x0a);
	  lcd_dat(0x82);
	  lcd_dat(0x27);
	  lcd_dat(0x00);
	  lcd_cmd(0x11); //sleep out
	  msleep(100);
	  lcd_cmd(0x29); // display on
	  msleep(100);
	  lcd_cmd(0x2c); //memory write	  
	} else if( my_strncmp( subcmd, "tp1", 8 ) == 0 ) {
	  lcd_cmd(0x2a); // column set
	  lcd_dat(0x00);
	  lcd_dat(0x00);
	  lcd_dat(0x00);
	  lcd_dat(0xEF);
	  lcd_cmd(0x2b); // page address set
	  lcd_dat(0x00);
	  lcd_dat(0x00);
	  lcd_dat(0x01);
	  lcd_dat(0x3F);

	  lcd_cmd(0x2c); //memory write	  
	  for( i = 0; i < 16384; i++ ) {
	    lcd_dat((uint16_t) i);
	  }
	} else {
	  printf( "lcd commands: su (setup timing), dump (dump registers), run, stop, init, tp1\n" );
	}

	return 0;
}

