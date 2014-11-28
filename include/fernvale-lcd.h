#ifndef __FV_LCD_H__
#define __FV_LCD_H__

// don't take values in here as gospel. some may be incorrect due to
// misinterpertation of reverse engineered values

#define LCD_CTRL_ADDR 0xa0450000

#define LCD_STATUS_REG				(LCD_CTRL_ADDR+0x0000)
#define LCD_STATUS_RUN_BIT			(0x1)
#define LCD_STATUS_WAIT_CMDQ_BIT		(0x2)
#define LCD_STATUS_WAIT_HTT_BIT			(0x8)
#define LCD_STATUS_TE_PENDING_BIT		(0x10)
#define LCD_STATUS_BUSY_BIT			(0x20)
#define LCD_STATUS_GMC_REQ_BIT			(0x40)

#define LCD_INT_ENA_REG				(LCD_CTRL_ADDR+0x0004)
#define LCD_INT_ENA_TRIG_BIT			(0x1)
#define LCD_INT_ENA_REG_TRIG_BIT		(0x2)
#define LCD_INT_ENA_CMD_TRIG_BIT		(0x4)
#define LCD_INT_ENA_HTT_TRIG_BIT		(0x10)
#define LCD_INT_ENA_HSYNC_TRIG_BIT		(0x20)
#define LCD_INT_ENA_VSYNC_TRIG_BIT		(0x20)

#define LCD_INT_STAT_REG			(LCD_CTRL_ADDR+0x0008)
#define LCD_INT_STAT_DONE_BIT			(0x1)

#define LCD_RUN_REG				(LCD_CTRL_ADDR+0x000C)
#define LCD_RUN_BIT				(0x8000)

#define LCD_RESET_REG				(LCD_CTRL_ADDR+0x0010)
#define LCD_RESET_MASK				(1 << 0)
#define LCD_RESET_SET				(0 << 0)
#define LCD_RESET_CLEAR				(1 << 0)

#define LCD_PAR0_CFG_REG			(LCD_CTRL_ADDR+0x0030)
#define LCD_PAR1_CFG_REG			(LCD_CTRL_ADDR+0x0034)
#define LCD_PAR_CFG_WR_WAIT_CYC_MASK		(0x3F)
#define LCD_PAR_CFG_WR_WAIT_CYC_BIT		(0)
#define LCD_PAR_CFG_WR_TSU_MASK			(0xF00)
#define LCD_PAR_CFG_WR_TSU_BIT			(8)
#define LCD_PAR_CFG_WR_TH_MASK			(0xF000)
#define LCD_PAR_CFG_WR_TH_BIT			(12)
#define LCD_PAR_CFG_RD_LATENCY_CYC_MASK		(0x3F0000)
#define LCD_PAR_CFG_RD_LATENCY_CYC_BIT		(16)
#define LCD_PAR_CFG_RD_TSU_MASK			(0xF000000)
#define LCD_PAR_CFG_RD_TSU_BIT			(24)
#define LCD_PAR_CFG_RD_TH_MASK			(0xF0000000)
#define LCD_PAR_CFG_RD_TH_BIT			(28)

#define LCD_PAR_DATA_WIDTH_REG			(LCD_CTRL_ADDR+0x003C)
#define LCD_PAR_BUS_WIDTH0_MASK			(0x7)
#define LCD_PAR_BUS_WIDTH0_BIT			(0)
#define LCD_PAR_BUS_WIDTH1_MASK			(0x70)
#define LCD_PAR_BUS_WIDTH1_BIT			(4)
#define LCD_PAR_W2W_WAIT0_MASK			(0xF0000)
#define LCD_PAR_W2W_WAIT0_BIT			(16)
#define LCD_PAR_W2W_WAIT1_MASK			(0xF00000)
#define LCD_PAR_W2W_WAIT1_BIT			(20)
#define LCD_PAR_BUS_WIDTH_8BIT			(0)
#define LCD_PAR_BUS_WIDTH_9BIT			(1)
#define LCD_PAR_BUS_WIDTH_16BIT			(2)
#define LCD_PAR_BUS_WIDTH_18BIT			(3)

#define LCD_TEARING_SYNC_CALC_REG		(LCD_CTRL_ADDR+0x0044)

#define LCD_TEARING_LCD_SIZE_REG		(LCD_CTRL_ADDR+0x0048)
#define LCD_TEARING_LCD_SIZE_HTIME_SHIFT	0
#define LCD_TEARING_LCD_SIZE_HTIME_MASK		0x3ff
#define LCD_TEARING_LCD_SIZE_VTIME_SHIFT	16
#define LCD_TEARING_LCD_SIZE_VTIME_MASK		0x0fff0000

#define LCD_TEARING_SYNC_CNT_REG		(LCD_CTRL_ADDR+0x004C)

#define LCD_TEARING_REG				(LCD_CTRL_ADDR+0x0050) 
#define LCD_TEARING_ENABLE			(1 << 0)
#define LCD_TEARING_POLARITY			(1 << 1)
#define LCD_TEARING_MODE_VSYNC			(1 << 2) /* Might be inverted */
#define LCD_TEARING_MODE_HSYNC			(0 << 2) /* Might be inverted */
#define LCD_TEARING_SW_FORCE_BIT		(1 << 15)

#define LCD_GMC_CTRL_REG			(LCD_CTRL_ADDR+0x0054)
#define LCD_GMC_CTRL_BURST_4BYTE		(0 << 0)
#define LCD_GMC_CTRL_BURST_16BYTE		(2 << 0)
#define LCD_GMC_CTRL_BURST_32BYTE		(3 << 0)
#define LCD_GMC_CTRL_BURST_64BYTE		(4 << 0)
#define LCD_GMC_CTRL_ENABLE			(1 << 4)
#define LCD_GMC_CTRL_PERIOD_MASK		(0xffff0000)
#define LCD_GMC_CTRL_PERIOD_SHIFT		16

#define LCD_AUTOCOPY_CTRL_REG			(LCD_CTRL_ADDR+0x0080)
// should be:
//  0 - BGR
//  0 - significance (?)
//  1 - padding on MSBs
//  010 - RGB565  (100 for RGB888, 011 for RGB66)
//  10  - 9-bit interface
//  000000 - command (no commands now)
// 0  -- disable W2M
// 0  -- enable commands
// 00000010 -- 2 cycle waiting period (may be ok to set 0 but for now set to 2)
// 1  -- send residue mode, residue per frame
// 0
// 1  -- enable frame update counter. why not?
// 0
// 1000 -- enable layer 0 only
// 1000 0101 0000 0010 1000 0000 1001 0100 = 0x85028094

#define LCD_AUTOCOPY_CTRL_FORMAT_MASK		0x020000FF

/* Pixel ordering */
#define LCD_AUTOCOPY_CTRL_FORMAT_RGB		(0 << 0)
#define LCD_AUTOCOPY_CTRL_FORMAT_BGR		(1 << 0)

/* Format when sending across wire */
#define LCD_AUTOCOPY_CTRL_FORMAT_MSB		(0 << 1)
#define LCD_AUTOCOPY_CTRL_FORMAT_LSB		(1 << 1)

/* If padding is needed, where to add padding */
#define LCD_AUTOCOPY_CTRL_FORMAT_PAD_LSB	(0 << 2)
#define LCD_AUTOCOPY_CTRL_FORMAT_PAD_MSB	(1 << 2)

#define LCD_AUTOCOPY_CTRL_FORMAT_RGB332		(0b000 << 3)
#define LCD_AUTOCOPY_CTRL_FORMAT_RGB444		(0b001 << 3)
#define LCD_AUTOCOPY_CTRL_FORMAT_RGB565		(0b010 << 3)
#define LCD_AUTOCOPY_CTRL_FORMAT_RGB666		(0b011 << 3)
#define LCD_AUTOCOPY_CTRL_FORMAT_RGB888		(0b100 << 3)

#define LCD_AUTOCOPY_CTRL_FORMAT_IFACE_8BIT	(0b00 << 6)
#define LCD_AUTOCOPY_CTRL_FORMAT_IFACE_16BIT	(0b01 << 6)
#define LCD_AUTOCOPY_CTRL_FORMAT_IFACE_9BIT	(0b10 << 6)
#define LCD_AUTOCOPY_CTRL_FORMAT_IFACE_18BIT	(0b11 << 6)

/* Support prefixing LCD data with command/data bytes */
#define LCD_AUTOCOPY_CTRL_CMD_COUNT_SHIFT	8
#define LCD_AUTOCOPY_CTRL_CMD_COUNT_MASK	0x00003f00

/* Enable writing to memory, in addition to writing to LCD module */
#define LCD_AUTOCOPY_CTRL_W2M			(1 << 14)

/* Enable sending command bytes. */
#define LCD_AUTOCOPY_CTRL_ENC			(1 << 15)

/* Waiting period between two consecutive data/command transfers */
#define LCD_AUTOCOPY_CTRL_PERIOD_SHIFT		16
#define LCD_AUTOCOPY_CTRL_PERIOD_MASK		0x00ff0000

/* Send residue per frame */
#define LCD_AUTOCOPY_CTRL_SEND_RESIDUE		(1 << 24)

#define LCD_AUTOCOPY_CTRL_COLOR_MODE		(1 << 26)

#define LCD_AUTOCOPY_CTRL_EN3			(1 << 28)
#define LCD_AUTOCOPY_CTRL_EN2			(1 << 29)
#define LCD_AUTOCOPY_CTRL_EN1			(1 << 30)
#define LCD_AUTOCOPY_CTRL_EN0			(1 << 31)
#define LCD_AUTOCOPY_CTRL_LAYER_MASK		0xf0000000

/* Set to 0, uint 32 */
#define LCD_AUTOCOPY_OFFSET_REG			(LCD_CTRL_ADDR+0x0084)

/* Address to write commands to, when running with LCD_AUTOCOPY_CTRL_ENC */
#define LCD_AUTOCOPY_CMD_ADDR_REG		(LCD_CTRL_ADDR+0x0088)

/* Address to write data to, when running with LCD_AUTOCOPY_CTRL_ENC */
#define LCD_AUTOCOPY_DATA_ADDR_REG		(LCD_CTRL_ADDR+0x008C)

/* Set to 0x014000F0, uint32 (320x240) */
#define LCD_AUTOCOPY_SIZE_REG			(LCD_CTRL_ADDR+0x0090)

/* Set to 0x80008000 (slight greenish) */
#define LCD_AUTOCOPY_BG_COLOR_REG		(LCD_CTRL_ADDR+0x009C)

#define LCD_LAYER0_CTRL_REG			(LCD_CTRL_ADDR+0x00B0)
#define LCD_LAYER_CTRL_OPA_MASK			(0xff)
#define LCD_LAYER_CTRL_OPA_SHIFT		0
#define LCD_LAYER_CTRL_OPA_EN			(1 << 8)
#define LCD_LAYER_CTRL_ROT0			(0 << 11)
#define LCD_LAYER_CTRL_ROT90			(1 << 11)
#define LCD_LAYER_CTRL_ROT180			(2 << 11)
#define LCD_LAYER_CTRL_ROT270			(3 << 11)
#define LCD_LAYER_CTRL_HFLIP			(1 << 13)
#define LCD_LAYER_CTRL_SRC_KEY_EN		(1 << 14)
#define LCD_LAYER_CTRL_DST_KEY_EN		(1 << 14)
#define LCD_LAYER_CTRL_DATA_SWAP_EN		(1 << 16)
#define LCD_LAYER_CTRL_DITHER_EN		(1 << 18)
#define LCD_LAYER_CTRL_CLRDPT_8BPP		(0 << 20)
#define LCD_LAYER_CTRL_CLRDPT_RGB565		(1 << 20)
#define LCD_LAYER_CTRL_CLRDPT_UYVY422		(2 << 20)
#define LCD_LAYER_CTRL_CLRDPT_RGB888		(3 << 20)
#define LCD_LAYER_CTRL_CLRDPT_ARGB8888		(4 << 20)
#define LCD_LAYER_CTRL_CLRDPT_PARGB8888		(5 << 20)
#define LCD_LAYER_CTRL_CLRDPT_XRGB		(6 << 20)
#define LCD_LAYER_CTRL_CLRDPT_ARGB6666		(7 << 20)
#define LCD_LAYER_CTRL_CLRDPT_PARGB6666		(8 << 20)
#define LCD_LAYER_CTRL_CLRDPT_MONO		(9 << 20)
#define LCD_LAYER_CTRL_SWP			(1 << 24)


#define LCD_LAYER0_SRC_KEY_REG			(LCD_CTRL_ADDR+0x00B4)
#define LCD_LAYER0_OFFSET_REG			(LCD_CTRL_ADDR+0x00B8)
#define LCD_LAYER0_SRC_ADDR_REG			(LCD_CTRL_ADDR+0x00BC)
#define LCD_LAYER0_SIZE_REG			(LCD_CTRL_ADDR+0x00C0)
#define LCD_LAYER0_MEM_OFFSET_REG		(LCD_CTRL_ADDR+0x00C8)
#define LCD_LAYER0_PITCH_REG			(LCD_CTRL_ADDR+0x00CC)

// there's 3 more layers, but I'm happy to just get one working for now...

#define LCD_FRAME_COUNTER_CON_REG		(LCD_CTRL_ADDR+0x0220)
#define LCD_FRAME_COUNTER_CON_START		(1 << 0)
#define LCD_FRAME_COUNTER_CON_STOP		(1 << 1)
#define LCD_FRAME_COUNTER_REG			(LCD_CTRL_ADDR+0x0224)

#define LCD_FREERUN_CTRL_REG			(LCD_CTRL_ADDR+0x0240)
#define LCD_FREERUN_CTRL_ENABLE_DBI		(1 << 0)
#define LCD_FREERUN_CTRL_ENABLE_GMC		(1 << 1)

#define LCD_FREERUN_RATE_REG			(LCD_CTRL_ADDR+0x0244)
#define LCD_FREERUN_RATE_MASK			(0x3ff)

#define LCD_FREERUN_DBI_THRESH_REG		(LCD_CTRL_ADDR+0x0248)
#define LCD_FREERUN_DBI_THRESH_LOW_SHIFT	0
#define LCD_FREERUN_DBI_THRESH_LOW_MASK		(0xffff)
#define LCD_FREERUN_DBI_THRESH_HIGH_SHIFT	16
#define LCD_FREERUN_DBI_THRESH_HIGH_MASK	(0xffff0000)

#define LCD_FREERUN_GMC_THRESH_REG		(LCD_CTRL_ADDR+0x0248)
#define LCD_FREERUN_GMC_THRESH_LOW_SHIFT	0
#define LCD_FREERUN_GMC_THRESH_LOW_MASK		(0xffff)
#define LCD_FREERUN_GMC_THRESH_HIGH_SHIFT	16
#define LCD_FREERUN_GMC_THRESH_HIGH_MASK	(0xffff0000)

#define LCD_CMD_LIST_ADDR			(LCD_CTRL_ADDR+0x0C00)

#define LCD_PAR0_CMD_PORT_REG			(LCD_CTRL_ADDR+0x0F00)
#define LCD_PAR0_DAT_PORT_REG			(LCD_CTRL_ADDR+0x0F10)
#define LCD_PAR1_CMD_PORT_REG			(LCD_CTRL_ADDR+0x0F20)
#define LCD_PAR1_DAT_PORT_REG			(LCD_CTRL_ADDR+0x0F30)

#endif /* __FV_LCD_H__ */
