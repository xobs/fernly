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

#define LCD_STATUS_REG               (LCD_CTRL_ADDR + 0x0000)
#define LCD_STATUS_RUN_BIT           (0x1)
#define LCD_STATUS_WAIT_CMDQ_BIT     (0x2)
#define LCD_STATUS_WAIT_HTT_BIT      (0x8)
#define LCD_STATUS_TE_PENDING_BIT    (0x10)
#define LCD_STATUS_BUSY_BIT          (0x20)
#define LCD_STATUS_GMC_REQ_BIT       (0x40)

#define LCD_INT_ENA_REG              (LCD_CTRL_ADDR + 0x0004)
#define LCD_INT_ENA_TRIG_BIT         (0x1)
#define LCD_INT_ENA_REG_TRIG_BIT     (0x2)
#define LCD_INT_ENA_CMD_TRIG_BIT     (0x4)
#define LCD_INT_ENA_HTT_TRIG_BIT     (0x10)
#define LCD_INT_ENA_HSYNC_TRIG_BIT   (0x20)
#define LCD_INT_ENA_VSYNC_TRIG_BIT   (0x20)

#define LCD_INT_STAT_REG             (LCD_CTRL_ADDR + 0x0008)
#define LCD_INT_STAT_DONE_BIT        (0x1)

#define LCD_RUN_REG                  (LCD_CTRL_ADDR + 0x000C)
#define LCD_RUN_BIT                  (0x8000)

#define LCD_RESET_REG                (LCD_CTRL_ADDR + 0x0010)
#define LCD_RESET_BIT                (0x1)  // check polarity

#define LCD_PAR_DATA_WIDTH_REG       (LCD_CTRL_ADDR + 0x003C)
#define LCD_PAR_BUS_WIDTH0_MASK      (0x7)
#define LCD_PAR_BUS_WIDTH0_BIT       (0)
#define LCD_PAR_BUS_WIDTH1_MASK      (0x70)
#define LCD_PAR_BUS_WIDTH1_BIT       (4)
#define LCD_PAR_WAIT0_MASK           (0xF0000)
#define LCD_PAR_WAIT0_BIT            (16)
#define LCD_PAR_WAIT1_MASK           (0xF00000)
#define LCD_PAR_WAIT1_BIT            (20)
#define LCD_PAR_BUS_WIDTH_8BIT       (0)  // valid values for LCD_PAR_BUS_WIDTHn
#define LCD_PAR_BUS_WIDTH_9BIT       (1)
#define LCD_PAR_BUS_WIDTH_16BIT      (2)
#define LCD_PAR_BUS_WIDTH_18BIT      (3)


#define LCD_TEARING_CON_REG               (LCD_CTRL_ADDR + 0x0050) 
#define LCD_TEARING_EN_BIT                (0x1)
#define LCD_TEARING_SYNCEDGE_BIT          (0x2)
#define LCD_TEARING_SW_FORCE_BIT          (0x8000)

#endif /* __FV_LCD_H__ */
