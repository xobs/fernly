#ifndef __FV_EMI_H__
#define __FV_EMI_H__

/* Register names based off of:
 * https://github.com/dragonpt/lenovo_A390_mt6577_uboot_kernel/blob
 *	/master/mediatek/custom
 *	/huaqin77_cu_a510_ics2/preloader/inc/mt6577_emi_reg.h
*/

/* Note: MT6260 has only bank 1 */
#define EMI_CTRL_ADDR 0xa0050000
#define EMI_CTRL_CONA (EMI_CTRL_ADDR + 0x00) /* Bank 0 control (low) */
#define EMI_CTRL_CONB (EMI_CTRL_ADDR + 0x08) /* Bank 1 control (low) */
#define EMI_CTRL_CONC (EMI_CTRL_ADDR + 0x10) /* Bank 2 control (low) */
#define EMI_CTRL_COND (EMI_CTRL_ADDR + 0x18) /* Bank 3 control (low) */
#define EMI_CTRL_CONE (EMI_CTRL_ADDR + 0x20) /* Bank 0 control (high) */
#define EMI_CTRL_CONF (EMI_CTRL_ADDR + 0x28) /* Bank 1 control (high) */
#define EMI_CTRL_CONG (EMI_CTRL_ADDR + 0x30) /* Bank 2 control (high) */
#define EMI_CTRL_CONH (EMI_CTRL_ADDR + 0x38) /* Bank 3 control (high) */
#define EMI_CTRL_CONI (EMI_CTRL_ADDR + 0x40) /* Bank 0 control for mDDR */
#define EMI_CTRL_CONJ (EMI_CTRL_ADDR + 0x48) /* Bank 1 control for mDDR */
#define EMI_CTRL_CONK (EMI_CTRL_ADDR + 0x50) /* Bank 2 control for mDDR */
#define EMI_CTRL_CONL (EMI_CTRL_ADDR + 0x58) /* Bank 3 control for mDDR */
#define EMI_CTRL_CONM (EMI_CTRL_ADDR + 0x60) /* */
#define EMI_CTRL_GENA (EMI_CTRL_ADDR + 0x70) /* General control register 0 */
#define EMI_CTRL_GENB (EMI_CTRL_ADDR + 0x78) /* General control register 1 */
#define EMI_CTRL_ADMUX (EMI_CTRL_ADDR + 0x80) /* Something involving GPIO mux */
#define EMI_CTRL_RDCT (EMI_CTRL_ADDR + 0x88)
#define EMI_CTRL_DLLV (EMI_CTRL_ADDR + 0x90)
#define EMI_CTRL_IDLA (EMI_CTRL_ADDR + 0xc0)
#define EMI_CTRL_IDLB (EMI_CTRL_ADDR + 0xc8)
#define EMI_CTRL_IDLC (EMI_CTRL_ADDR + 0xd0)
#define EMI_CTRL_IDLD (EMI_CTRL_ADDR + 0xd8)
#define EMI_CTRL_IDLE (EMI_CTRL_ADDR + 0xe0)
#define EMI_CTRL_ODLA (EMI_CTRL_ADDR + 0xe8)
#define EMI_CTRL_ODLB (EMI_CTRL_ADDR + 0xf0)
#define EMI_CTRL_ODLC (EMI_CTRL_ADDR + 0xf8)
#define EMI_CTRL_ODLD (EMI_CTRL_ADDR + 0x100)
#define EMI_CTRL_ODLE (EMI_CTRL_ADDR + 0x108)
#define EMI_CTRL_ODLF (EMI_CTRL_ADDR + 0x110)
#define EMI_CTRL_IOA (EMI_CTRL_ADDR + 0x130)
#define EMI_CTRL_IOB (EMI_CTRL_ADDR + 0x138)
#define EMI_CTRL_DSRAM (EMI_CTRL_ADDR + 0x150)
#define EMI_CTRL_ARBA (EMI_CTRL_ADDR + 0x170)
#define EMI_CTRL_ARBB (EMI_CTRL_ADDR + 0x178)
#define EMI_CTRL_ARBC (EMI_CTRL_ADDR + 0x180)
#define EMI_CTRL_SLCT (EMI_CTRL_ADDR + 0x198)
#define EMI_CTRL_ABCT (EMI_CTRL_ADDR + 0x1a0)
#define EMI_CTRL_BMEN (EMI_CTRL_ADDR + 0x200)
#define EMI_CTRL_BCNT (EMI_CTRL_ADDR + 0x208)
#define EMI_CTRL_TACT (EMI_CTRL_ADDR + 0x210)
#define EMI_CTRL_TSCT (EMI_CTRL_ADDR + 0x218)
#define EMI_CTRL_WACT (EMI_CTRL_ADDR + 0x220)
#define EMI_CTRL_WSCT (EMI_CTRL_ADDR + 0x228)
#define EMI_CTRL_BACT (EMI_CTRL_ADDR + 0x230)
#define EMI_CTRL_BSCT0 (EMI_CTRL_ADDR + 0x238)
#define EMI_CTRL_BSCT1 (EMI_CTRL_ADDR + 0x240)
#define EMI_CTRL_TTYPE1 (EMI_CTRL_ADDR + 0x280)
#define EMI_CTRL_TTYPE2 (EMI_CTRL_ADDR + 0x288)
#define EMI_CTRL_TTYPE3 (EMI_CTRL_ADDR + 0x290)
#define EMI_CTRL_TTYPE4 (EMI_CTRL_ADDR + 0x298)
#define EMI_CTRL_TTYPE5 (EMI_CTRL_ADDR + 0x2a0)
#define EMI_CTRL_TTYPE6 (EMI_CTRL_ADDR + 0x2a8)
#define EMI_CTRL_TTYPE7 (EMI_CTRL_ADDR + 0x2b0)
#define EMI_CTRL_TTYPE8 (EMI_CTRL_ADDR + 0x2b8)
#define EMI_CTRL_TTYPE9 (EMI_CTRL_ADDR + 0x2c0)
#define EMI_CTRL_TTYPE10 (EMI_CTRL_ADDR + 0x2c8)
#define EMI_CTRL_TTYPE11 (EMI_CTRL_ADDR + 0x2d0)
#define EMI_CTRL_TTYPE12 (EMI_CTRL_ADDR + 0x2d8)
#define EMI_CTRL_TTYPE13 (EMI_CTRL_ADDR + 0x2e0)
#define EMI_CTRL_TTYPE14 (EMI_CTRL_ADDR + 0x2e8)
#define EMI_CTRL_TTYPE15 (EMI_CTRL_ADDR + 0x2f0)
#define EMI_CTRL_TTYPE16 (EMI_CTRL_ADDR + 0x2f8)

#define EMI_CTRL_MBISTA (EMI_CTRL_ADDR + 0x300)
#define EMI_CTRL_MBISTA_PATTERN_SHIFT 16
#define EMI_CTRL_MBISTA_PATTERN_MASK 0xffff0000

#define EMI_CTRL_MBISTB (EMI_CTRL_ADDR + 0x308)
#define EMI_CTRL_MBISTB_START_KEY 0x325
#define EMI_CTRL_MBISTB_RANDOMIZE (1 << 12)
#define EMI_CTRL_MBISTB_INVERT (1 << 13)

#define EMI_CTRL_MBISTC (EMI_CTRL_ADDR + 0x310)

#define EMI_CTRL_MBISTD (EMI_CTRL_ADDR + 0x318)
#define EMI_CTRL_MBISTD_FAILURE (1 << 0)
#define EMI_CTRL_MBISTD_FINISHED (1 << 1)



#define EMI_CTRL_TEST (EMI_CTRL_ADDR + 0x330)

#define EMI_CTRL_REMAP (0xA0510000)

#endif /* __FV_EMI_H__ */
