#ifndef __FV_LCD_H__
#define __FV_LCD_H__

// don't take values in here as gospel. some may be incorrect due to
// misinterpertation of reverse engineered values

#define LCD_CTRL_ADDR 0xa0450000

#define LCD_PAR0_CMD_PORT_REG   (LCD_CTRL_ADDR + 0x0F00)
#define LCD_PAR0_DAT_PORT_REG   (LCD_CTRL_ADDR + 0x0F10)
#define LCD_PAR1_CMD_PORT_REG   (LCD_CTRL_ADDR + 0x0F20)
#define LCD_PAR1_DAT_PORT_REG   (LCD_CTRL_ADDR + 0x0F30)

#define LCD_PAR0_CFG_REG        (LCD_CTRL_ADDR + 0x0030)
#define LCD_PAR1_CFG_REG        (LCD_CTRL_ADDR + 0x0034)
#define LCD_PAR_CFG_WR_WAIT_CYC_MASK    (0x3F)
#define LCD_PAR_CFG_WR_WAIT_CYC_BIT     (0)
#define LCD_PAR_CFG_WR_TSU_MASK         (0xF00)
#define LCD_PAR_CFG_WR_TSU_BIT          (8)
#define LCD_PAR_CFG_WR_TH_MASK          (0xF000)
#define LCD_PAR_CFG_WR_TH_BIT           (12)
#define LCD_PAR_CFG_RD_LATENCY_CYC_MASK (0x3F0000)
#define LCD_PAR_CFG_RD_LATENCY_CYC_BIT  (16)
#define LCD_PAR_CFG_RD_TSU_MASK         (0xF000000)
#define LCD_PAR_CFG_RD_TSU_BIT          (24)
#define LCD_PAR_CFG_RD_TH_MASK          (0xF0000000)
#define LCD_PAR_CFG_RD_TH_BIT           (28)

#define LCD_STATUS_REG               (LCD_CTRL_ADDR + 0x0000)  // 16-bit
#define LCD_STATUS_RUN_BIT           (0x1)
#define LCD_STATUS_WAIT_CMDQ_BIT     (0x2)
#define LCD_STATUS_WAIT_HTT_BIT      (0x8)
#define LCD_STATUS_TE_PENDING_BIT    (0x10)
#define LCD_STATUS_BUSY_BIT          (0x20)
#define LCD_STATUS_GMC_REQ_BIT       (0x40)

#define LCD_INT_ENA_REG              (LCD_CTRL_ADDR + 0x0004)  // 16-bit
#define LCD_INT_ENA_TRIG_BIT         (0x1)
#define LCD_INT_ENA_REG_TRIG_BIT     (0x2)
#define LCD_INT_ENA_CMD_TRIG_BIT     (0x4)
#define LCD_INT_ENA_HTT_TRIG_BIT     (0x10)
#define LCD_INT_ENA_HSYNC_TRIG_BIT   (0x20)
#define LCD_INT_ENA_VSYNC_TRIG_BIT   (0x20)

#define LCD_INT_STAT_REG             (LCD_CTRL_ADDR + 0x0008)  // 16-bit
#define LCD_INT_STAT_DONE_BIT        (0x1)

#define LCD_RUN_REG                  (LCD_CTRL_ADDR + 0x000C)  // 16-bit
#define LCD_RUN_BIT                  (0x8000)

#define LCD_RESET_REG                (LCD_CTRL_ADDR + 0x0010)  // 16-bit
#define LCD_RESET_BIT                (0x1)  // check polarity

#define LCD_PAR_DATA_WIDTH_REG       (LCD_CTRL_ADDR + 0x003C)
#define LCD_PAR_BUS_WIDTH0_MASK      (0x7)
#define LCD_PAR_BUS_WIDTH0_BIT       (0)
#define LCD_PAR_BUS_WIDTH1_MASK      (0x70)
#define LCD_PAR_BUS_WIDTH1_BIT       (4)
#define LCD_PAR_W2W_WAIT0_MASK       (0xF0000)  // write 2 write delay
#define LCD_PAR_W2W_WAIT0_BIT        (16)
#define LCD_PAR_W2W_WAIT1_MASK       (0xF00000)
#define LCD_PAR_W2W_WAIT1_BIT        (20)
#define LCD_PAR_BUS_WIDTH_8BIT       (0)  // valid values for LCD_PAR_BUS_WIDTHn
#define LCD_PAR_BUS_WIDTH_9BIT       (1)
#define LCD_PAR_BUS_WIDTH_16BIT      (2)
#define LCD_PAR_BUS_WIDTH_18BIT      (3)


#define LCD_TEARING_CON_REG               (LCD_CTRL_ADDR + 0x0050) 
#define LCD_TEARING_EN_BIT                (0x1)
#define LCD_TEARING_SYNCEDGE_BIT          (0x2)
#define LCD_TEARING_SW_FORCE_BIT          (0x8000)

#define LCD_AUTOCOPY_CTRL_REG            (LCD_CTRL_ADDR+0x0080)
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

#define LCD_AUTOCOPY_CTRL_SEND_RES_MOD_MASK      0x01000000
#define LCD_AUTOCOPY_CTRL_PERIOD_MASK            0x00FF0000
#define LCD_AUTOCOPY_CTRL_LAYER_MASK             0xF0000000
#define LCD_AUTOCOPY_CTRL_CMD_ENABLE_BIT         0x00008000
#define LCD_AUTOCOPY_CTRL_DATA_FORMAT_MASK       0x020000FF   ///24 bit interface support

#define LCD_AUTOCOPY_CTRL_CMDQ_SEL_BIT           0  					
#define LCD_AUTOCOPY_CTRL_CMD_NUMBER_MASK        0x00003F00

#define LCD_AUTOCOPY_OFFSET_REG          (LCD_CTRL_ADDR+0x0084) // set to 0, uint 32
#define LCD_AUTOCOPY_CMD_ADDR_REG        (LCD_CTRL_ADDR+0x0088) // set to 0x4000, uint 16
#define LCD_AUTOCOPY_DATA_ADDR_REG       (LCD_CTRL_ADDR+0x008C) // set to 0x4100, uint 16
#define LCD_AUTOCOPY_SIZE_REG            (LCD_CTRL_ADDR+0x0090) // set to 0x014000F0, uint32 (320x240)
#define LCD_AUTOCOPY_BG_COLOR_REG        (LCD_CTRL_ADDR+0x009C) // set to 0x80008000 (slight greenish)

#define LCD_LAYER0_CTRL_REG         (LCD_CTRL_ADDR+0x00B0) // set to 0x0
#define LCD_LAYER0_SRC_KEY_REG      (LCD_CTRL_ADDR+0x00B4)
#define LCD_LAYER0_OFFSET_REG       (LCD_CTRL_ADDR+0x00B8) // set to 0x0
#define LCD_LAYER0_BUFF_ADDR_REG    (LCD_CTRL_ADDR+0x00BC) // set to 0x0
#define LCD_LAYER0_SIZE_REG         (LCD_CTRL_ADDR+0x00C0) // set to 0x014000f0
#define LCD_LAYER0_MEM_OFFSET_REG   (LCD_CTRL_ADDR+0x00C8) // set to 0x0
#define LCD_LAYER0_MEM_PITCH_REG    (LCD_CTRL_ADDR+0x00CC) // set to 0x1e0

// there's 3 more layers, but I'm happy to just get one working for now...

#define LCD_CMD_LIST_ADDR           (LCD_CTRL_ADDR + 0x0C00)

#endif /* __FV_LCD_H__ */
