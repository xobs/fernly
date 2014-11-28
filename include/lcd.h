#ifndef __LCD_H__
#define __LCD_H__

typedef uint16_t pixel_t;

/* Current LCD is rgb565, but this function can change for different panels */
static inline pixel_t rgb(unsigned r, unsigned g, unsigned b)
{
	uint16_t c = (((r >> 3) & 0x1f) << 11) |
		     (((g >> 2) & 0x3f) <<  5) |
		     (((b >> 3) & 0x1f)      );
	//c = ((c >> 8) & 0x00ff) | ((c << 8) & 0xff00);
	return c;
}

int lcd_init(void);
int lcd_run(void);
int lcd_stop(void);

pixel_t *lcd_fb(void);
uint32_t lcd_width(void);
uint32_t lcd_height(void);
uint32_t lcd_bpp(void);

void lcd_addpixel(pixel_t px);

#endif /* __LCD_H__ */
