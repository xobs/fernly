#ifndef __FV_BL_H__
#define __FV_BL_H__

#define BLLED_MAX_LEVEL      5

#define BLLED_GANG_REG0             (0xA0700000 + 0x0C00)
#define BLLED_GANG_REG2             (0xA0700000 + 0x0C08)

#define BLLED_BANKS          4
#define BLLED_REG0_BANK(n)      (0xA0700000 + 0x0C10 + ((n & 3) << 4))
#define BLLED_REG1_BANK(n)      (0xA0700000 + 0x0C14 + ((n & 3) << 4))

/// common across BLLED BANK 0-3
// uses BLLED_REG0_BANK(n) macro
#define BLLED_REG0_STATUS_BIT           15          
#define BLLED_REG0_STATUS_MSK           (0x8000)

#define BLLED_REG0_CLKSEL_BIT           13
#define BLLED_REG0_CLKSEL_MSK           (0x2000)

#define BLLED_REG0_CLK_MODE_BIT         12
#define BLLED_REG0_CLK_MODE_MSK         (0x1000)

#define BLLED_REG0_DOUBLE_BIT           8
#define BLLED_REG0_DOUBLE_MSK           (0x100)

#define BLLED_REG0_STEP_BIT             4
#define BLLED_REG0_STEP_MSK             (0x70) // 3-bit step value

#define BLLED_REG0_CHOP_ENB_BIT         2
#define BLLED_REG0_CHOP_ENB_MSK         (0x4)

#define BLLED_REG0_MODE_BIT             1
#define BLLED_REG0_MODE_MSK             (0x2)

#define BLLED_REG0_EN_BIT               0
#define BLLED_REG0_EN_MSK               (0x1)

// uses BLLED_REG1_BANK(n) macro
#define BLLED_REG1_STP_MODE_BIT         1
#define BLLED_REG1_STP_MODE_MSK         (0x2)

#define BLLED_REG1_PHASE_MODE_BIT       0
#define BLLED_REG1_PHASE_MODE_MSK       (0x1)


//// gang-operations on all BLLEDs

#define BLLED_GANG_REG0_RSV_BIT         8
#define BLLED_GANG_REG0_RSV_MSK         (0xF00)  // 4 bits

#define BLLED_GANG_REG0_STEP_TC_BIT     4
#define BLLED_GANG_REG0_STEP_TC_MSK     (0x30)  // 2 bits

#define BLLED_GANG_REG0_PHASE_TC_BIT    0
#define BLLED_GANG_REG0_PHASE_TC_MSK    (0x3)   // 2 bits

#define BLLED_GANG_REG2_TRIM_SEL_BIT    8
#define BLLED_GANG_REG2_TRIM_SEL_MSK    (0x700)  // 3 bits

#define BLLED_GANG_REG2_TRIM_EN_BIT     0
#define BLLED_GANG_REG2_TRIM_EN_MSK     (0x1)


#endif /* __FV_BL_H__ */
