#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "fernvale-lcd.h"
#include "lcd.h"

#ifdef LCD_DEBUG
static void lcd_dump(void)
{
	/* Dump registers */
//	printf("LCD_PAR0_CMD_PORT:      %04x\n", readw(LCD_PAR0_CMD_PORT_REG));
//	printf("LCD_PAR0_DAT_PORT:      %04x\n", readw(LCD_PAR0_DAT_PORT_REG));
//	printf("LCD_PAR1_CMD_PORT:      %04x\n", readw(LCD_PAR1_CMD_PORT_REG));
//	printf("LCD_PAR1_DAT_PORT:      %04x\n", readw(LCD_PAR1_DAT_PORT_REG));
//	printf("LCD_PAR0_CFG:           %08x\n", readl(LCD_PAR0_CFG_REG));
//	printf("LCD_PAR1_CFG:           %08x\n", readl(LCD_PAR1_CFG_REG));
	printf("LCD_STATUS:             %04x\n", readw(LCD_STATUS_REG));
	printf("LCD_INT_ENA:            %04x\n", readw(LCD_INT_ENA_REG));
	printf("LCD_INT_STAT:           %04x\n", readw(LCD_INT_STAT_REG));
	printf("LCD_RUN:                %04x\n", readw(LCD_RUN_REG));
	printf("LCD_RESET:              %04x\n", readw(LCD_RESET_REG));
	printf("LCD_PAR_DATA_WIDTH:     %08x\n", readl(LCD_PAR_DATA_WIDTH_REG));
	printf("LCD_TEARING:            %08x\n", readl(LCD_TEARING_REG));
	printf("LCD_AUTOCOPY_CTRL:      %08x\n", readl(LCD_AUTOCOPY_CTRL_REG));
	printf("LCD_AUTOCOPY_OFFSET:    %08x\n", readl(LCD_AUTOCOPY_OFFSET_REG));
	printf("LCD_AUTOCOPY_SIZE:      %08x\n", readl(LCD_AUTOCOPY_SIZE_REG));
	printf("LCD_AUTOCOPY_CMD_ADDR:  %04x\n", readw(LCD_AUTOCOPY_CMD_ADDR_REG));
	printf("LCD_AUTOCOPY_DATA_ADDR: %04x\n", readw(LCD_AUTOCOPY_DATA_ADDR_REG));
	printf("LCD_LAYER0_CTRL:        %08x\n", readl(LCD_LAYER0_CTRL_REG));
	printf("LCD_LAYER0_OFFSET:      %08x\n", readl(LCD_LAYER0_OFFSET_REG));
	printf("LCD_LAYER0_SIZE:        %08x\n", readl(LCD_LAYER0_SIZE_REG));
	printf("LCD_LAYER0_SRC_ADDR:    %08x\n", readl(LCD_LAYER0_SRC_ADDR_REG));
//	printf("LCD_LAYER0_MEM_PITCH:   %08x\n", readl(LCD_LAYER0_MEM_PITCH_REG));
//	printf("LCD_FRAME_COUNTER_CON:  %08x\n", readl(LCD_FRAME_COUNTER_CON_REG));
//	printf("LCD_FRAME_COUNTER:      %08x\n", readl(LCD_FRAME_COUNTER_REG));
}
#endif /* LCD_DEBUG */

static int is_command(int argc, char **argv, const char *cmd)
{
	return ((argc > 0) && !_strcasecmp(argv[0], cmd));
}

static pixel_t color_wheel(int step) {
	step &= 255;
	if (step < 85) {
		return rgb(step * 3, 255 - step * 3, 0);
	}
	else if (step < 170) {
		step -= 85;
		return rgb(255 - step * 3, 0, step * 3);
	}
	else {
		step -= 170;
		return rgb(0, step * 3, 255 - step * 3);
	}
}

int cmd_lcd(int argc, char **argv)
{
	int i;
	int ret;

	if (is_command(argc, argv, "init")) {
		printf("Initializing LCD... ");
		ret = lcd_init();
		if (ret)
			printf("failed: %d\n", ret);
		else
			printf("Ok\n");
	}
#ifdef LCD_DEBUG
	else if (is_command(argc, argv, "dump")) {
		lcd_dump();
	}
#endif
	else if (is_command(argc, argv, "run")) {
		printf("Running LCD... ");
		ret = lcd_run();
		if (ret)
			printf("failed: %d\n", ret);
		else
			printf("Ok\n");
	}
	else if (is_command(argc, argv, "stop")) {
		printf("Stopping LCD... ");
		ret = lcd_stop();
		if (ret)
			printf("failed: %d\n", ret);
		else
			printf("Ok\n");
	}
	else if (is_command(argc, argv, "tpp1")) {
		int w = lcd_width();
		int h = lcd_height();
		int total = w * h;

		for (i = 0; i < total; i++)
			lcd_addpixel(i);
	}
	else if (is_command(argc, argv, "tpp2")) {
		int x, y;

		i = 0;
		for (y = 0; y < lcd_height(); y++)
			for (x = 0; x < lcd_width(); x++)
				lcd_addpixel(rgb(i++, 0, 0));
	}
	else if (is_command(argc, argv, "tpd")) {
		static int step = 0;
		pixel_t *fb;
		int x, y;
		int w, h;

		fb = lcd_fb();

		h = lcd_height();
		w = lcd_width();

		/* Stupid clear-screen */
		memset(fb, 0, w * h * lcd_bpp());

		printf("Width: %d  Height: %d\n", w, h);

		i = step++;
		fb = lcd_fb();
		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				/* Swap axes, to verify X and Y work */
				if (step & 1)
					*fb++ = color_wheel(y + step);
				else
					*fb++ = color_wheel(x + step);
			}
		}
		lcd_run();
	}
	else {
		printf("lcd sub-commands (usage: lcd [subcmd]):\n");
		printf("\tinit    Initialize LCD registers\n");
		printf("\trun     Transfer one frame of the LCD\n");
		printf("\tstop    Stop and reset LCD auto-update\n");
#ifdef LCD_DEBUG
		printf("\tdump    Dump current register list\n");
#endif
		printf("\ttpp1    Display bitbanged, PIO 'test pattern 1'\n");
		printf("\ttpp2    Display bitbanged, PIO 'test pattern 2'\n");
		printf("\ttpd     DMA test pattern (flips on each iteration)\n");
	}

	return 0;
}

