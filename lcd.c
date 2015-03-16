#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "fernvale-lcd.h"
#include "fernvale-gpio.h"
#include "fernvale-clockgate.h"
#include "lcd.h"

static pixel_t *fb = (pixel_t *)0x40000;
static const uint32_t fb_height = 320;
static const uint32_t fb_width = 240;
static const uint32_t fb_bpp = 2;

#define LCD_FORMAT (0 \
		| LCD_AUTOCOPY_CTRL_FORMAT_PAD_MSB \
		| LCD_AUTOCOPY_CTRL_FORMAT_RGB565 \
		| LCD_AUTOCOPY_CTRL_FORMAT_IFACE_9BIT \
		)

#define lcd_cmd(_cmd_) writew(_cmd_, LCD_PAR0_CMD_PORT_REG)
#define lcd_dat(_dat_) writew(_dat_, LCD_PAR0_DAT_PORT_REG)

/* Note these don't flush the DMA, you have to do this yourself
 * explicitly later on.
 *    [slot] is the order to execute.
 */
static void lcd_cmd_slot(uint16_t cmd, uint8_t slot)
{
	writel(cmd | 0x800000, LCD_CMD_LIST_ADDR + (slot * 4));
}
#pragma GCC diagnostic ignored "-Wunused-function"
static void lcd_dat_slot(uint16_t dat, uint8_t slot)
{
	writel(dat, LCD_CMD_LIST_ADDR + (slot * 4));
}
static void lcd_setup_gpio(void)
{
	/* LPCE0, LPTE0, LPRSTB */
	writel(readl(GPIO_CTRL_MODE5) & ~(GPIO_CTRL_MODE5_IO40_MASK |
					  GPIO_CTRL_MODE5_IO46_MASK |
					  GPIO_CTRL_MODE5_IO45_MASK),
					  GPIO_CTRL_MODE5);
	writel(readl(GPIO_CTRL_MODE5) | (GPIO_CTRL_MODE5_IO40_LPCE0B |
					 GPIO_CTRL_MODE5_IO45_LPTE0  |
					 GPIO_CTRL_MODE5_IO46_LPRSTB),
					 GPIO_CTRL_MODE5);

	/* NLD0-4, LWRB, LRDB, LPA0 */
	writel(readl(GPIO_CTRL_MODE4) & ~(GPIO_CTRL_MODE4_IO32_MASK |
					  GPIO_CTRL_MODE4_IO33_MASK |
					  GPIO_CTRL_MODE4_IO34_MASK |
					  GPIO_CTRL_MODE4_IO35_MASK |
					  GPIO_CTRL_MODE4_IO36_MASK |
					  GPIO_CTRL_MODE4_IO37_MASK |
					  GPIO_CTRL_MODE4_IO38_MASK |
					  GPIO_CTRL_MODE4_IO39_MASK),
					  GPIO_CTRL_MODE4);
	writel(readl(GPIO_CTRL_MODE4) | (GPIO_CTRL_MODE4_IO32_NLD4 |
					 GPIO_CTRL_MODE4_IO33_NLD3 |
					 GPIO_CTRL_MODE4_IO34_NLD2 |
					 GPIO_CTRL_MODE4_IO35_NLD1 |
					 GPIO_CTRL_MODE4_IO36_NLD0 |
					 GPIO_CTRL_MODE4_IO37_LWRB |
					 GPIO_CTRL_MODE4_IO38_LRDB |
					 GPIO_CTRL_MODE4_IO39_LPA0),
					 GPIO_CTRL_MODE4);

	/* NLD5-8 */
	writel(readl(GPIO_CTRL_MODE3) & ~(GPIO_CTRL_MODE3_IO28_MASK |
					  GPIO_CTRL_MODE3_IO29_MASK |
					  GPIO_CTRL_MODE3_IO30_MASK |
					  GPIO_CTRL_MODE3_IO31_MASK),
					  GPIO_CTRL_MODE3);
	writel(readl(GPIO_CTRL_MODE3) | (GPIO_CTRL_MODE3_IO28_NLD8 |
					 GPIO_CTRL_MODE3_IO29_NLD7 |
					 GPIO_CTRL_MODE3_IO30_NLD6 |
					 GPIO_CTRL_MODE3_IO31_NLD5),
					 GPIO_CTRL_MODE3);
}

static int lcd_setup(void)
{
	lcd_setup_gpio();
  
	/* Power up the LCD block */
	writel(CLKGATE_CTL0_LCD, CLKGATE_SYS_CTL0_CLR);

	_msleep(1);

	/* execute setup command
	 * we're on CS0
	 * our internal bus period is 166 MHz, or 6.25ns
	 * write cycle = 66ns = 11 cycles - 1 = 10
	 * write c22write su (tcs) = 15ns = 3 cycles
	 * write ce2write hold (tdht) = 10ns = 2 cycles - 1 = 1 
	 * read latency = 450 ns = 72 cycles, crop at 63 cycles
	 * read ce2read su (trdl - trcs) = 45-45 = 0 ns = 0 cycles
	 * read th = 90ns = 15 cycles (not quite clear, but best guess)
	 */
	writel( (10 << LCD_PAR_CFG_WR_WAIT_CYC_BIT) | 
		(3  << LCD_PAR_CFG_WR_TSU_BIT) |
		(1  << LCD_PAR_CFG_WR_TH_BIT) |
		/* this might need to be shorter?? */
		(63 << LCD_PAR_CFG_RD_LATENCY_CYC_BIT) |
		(0  << LCD_PAR_CFG_RD_TSU_BIT) |
		(15 << LCD_PAR_CFG_RD_TH_BIT),
		LCD_PAR0_CFG_REG);
  
	/* 9 bit width, tchw is 0 for this chipset
	 * (back2back writes allowed)
	 */
	writel( (0                      << LCD_PAR_W2W_WAIT0_BIT) |
		(LCD_PAR_BUS_WIDTH_9BIT << LCD_PAR_BUS_WIDTH0_BIT),
		LCD_PAR_DATA_WIDTH_REG);

	/* Set up tear control */
	//writel(LCD_TEARING_MODE_HSYNC | LCD_TEARING_ENABLE, LCD_TEARING_REG);
	//writel(0x0003000f, LCD_TEARING_LCD_SIZE_REG);
	//writel(4, LCD_TEARING_SYNC_CNT_REG);

	//writel(0x10000 | LCD_GMC_CTRL_ENABLE | 4, LCD_GMC_CTRL_REG);
	//writel(15, LCD_FREERUN_RATE_REG);
	//writel(20 << LCD_FREERUN_DBI_THRESH_HIGH_SHIFT, LCD_FREERUN_DBI_THRESH_REG);
	//writel(20 << LCD_FREERUN_GMC_THRESH_HIGH_SHIFT, LCD_FREERUN_GMC_THRESH_REG);

	/* Point freerunning DMA engine at parallel LCD registers */
	writew((uint16_t)LCD_PAR0_CMD_PORT_REG, LCD_AUTOCOPY_CMD_ADDR_REG);
	writew((uint16_t)LCD_PAR0_DAT_PORT_REG, LCD_AUTOCOPY_DATA_ADDR_REG);

	return 0;
}

static void lcd_panel_setup(void)
{
	writew(LCD_RESET_CLEAR, LCD_RESET_REG);
	_usleep(20000);
	writew(LCD_RESET_SET, LCD_RESET_REG);
	_msleep(20);
	writew(LCD_RESET_CLEAR, LCD_RESET_REG);
	_msleep(150);

	lcd_cmd(0x11); //Exit Sleep
	_msleep(50); // Delay 50ms
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
	_msleep(100);
	lcd_cmd(0x29); // display on
	_msleep(100);

	/* Memory Write -- prep for the first pixel to be written */
	lcd_cmd(0x2c);
}

/* Fill pre-frame command buffer.  These commands are sent out before
 * pixel data, whenever RUN is enabled.
 */
static void lcd_fill_cmd_buffer(void)
{
	int ncommands = 0;

	/* Memory write */
	lcd_cmd_slot(0x2c, ncommands++);

	/* Count the number of cmmands and add it to AUTOCOPY_CTRL */
	writel((readl(LCD_AUTOCOPY_CTRL_REG)
		& ~LCD_AUTOCOPY_CTRL_CMD_COUNT_MASK)
		| ((ncommands - 1) << LCD_AUTOCOPY_CTRL_CMD_COUNT_SHIFT),
		LCD_AUTOCOPY_CTRL_REG);
}

static int lcd_dma_setup(void)
{
	/* Set up AUTOCOPY (i.e. freerunning mode) */
	writel(LCD_FORMAT | (0x1f  << LCD_AUTOCOPY_CTRL_PERIOD_SHIFT),
		LCD_AUTOCOPY_CTRL_REG);
	writel((fb_height << 16) | (fb_width), LCD_AUTOCOPY_SIZE_REG);
	writel(0, LCD_AUTOCOPY_OFFSET_REG);

	writel((uint32_t)fb, LCD_LAYER0_SRC_ADDR_REG);
	writel(LCD_LAYER_CTRL_CLRDPT_RGB565, LCD_LAYER0_CTRL_REG);
	writel((fb_height << 16) | (fb_width), LCD_LAYER0_SIZE_REG);
	writel(fb_width * fb_bpp, LCD_LAYER0_PITCH_REG);
	writel(0, LCD_LAYER0_MEM_OFFSET_REG);
	writel(0, LCD_LAYER0_OFFSET_REG);
	writel(0, LCD_LAYER0_SRC_KEY_REG);

	writel(rgb(0, 255, 255), LCD_AUTOCOPY_BG_COLOR_REG);

	writel(readl(LCD_AUTOCOPY_CTRL_REG) | LCD_AUTOCOPY_CTRL_EN0,
			LCD_AUTOCOPY_CTRL_REG);

	/* Enable AUTOCOPY_CTRL command transfer */
	writel(readl(LCD_AUTOCOPY_CTRL_REG)
			| LCD_AUTOCOPY_CTRL_ENC
			| LCD_AUTOCOPY_CTRL_SEND_RESIDUE,
			LCD_AUTOCOPY_CTRL_REG);
	return 0;
}

int lcd_init(void)
{
	lcd_setup();
	lcd_panel_setup();
	lcd_dma_setup();
	return 0;
}

int lcd_run(void)
{
	writew(0, LCD_RUN_REG);

	/* Must refill the command buffer before sending another frame */
	lcd_fill_cmd_buffer();

	writew(LCD_RUN_BIT, LCD_RUN_REG);
	return 0;
}

int lcd_stop(void)
{
	writew(1, LCD_RUN_REG);
	writew(0, LCD_RUN_REG);
	return 0;
}

pixel_t *lcd_fb(void)
{
	return fb;
}

uint32_t lcd_width(void)
{
	return fb_width;
}

uint32_t lcd_height(void)
{
	return fb_height;
}

uint32_t lcd_bpp(void)
{
	return fb_bpp;
}

void lcd_addpixel(pixel_t px)
{
	lcd_dat(px >> 8);
	lcd_dat(px & 0xff);
}
