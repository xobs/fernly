#ifndef __FV_KBD_H__
#define __FV_KBD_H__

/// The BIG_LED is a 60mA drive-capable open-drain switch
/// it's useful for driving circuits such as an array of LEDs
/// used to illuminate a keyboard, hence its inclusion in the
/// keyboard section. However, on PCBs with no keyboard, it's
/// simply a big LED driver.

#define BIG_LED_ADDR  (0xA0700000 + 0x0C80)
#define BIG_LED_ON    (0x3)
#define BIG_LED_OFF   (0x0)

#define KBD_ADDR		0xA00D0000
#define KBD_STATUS		(KBD_ADDR + 0x0000)
#define KBD_MEM1		(KBD_ADDR + 0x4)
#define KBD_MEM2		(KBD_ADDR + 0x8)
#define KBD_MEM3		(KBD_ADDR + 0xc)
#define KBD_MEM4		(KBD_ADDR + 0x10)
#define KBD_MEM5		(KBD_ADDR + 0x14)
#define KBD_DEBOUNCING		(KBD_ADDR + 0x18)
#define KBD_SCAN_TIME_ADJ	(KBD_ADDR + 0x1c)
#define KBD_1OR2		(KBD_ADDR + 0x20)
#define KBD_ENABLE		(KBD_ADDR + 0x24)


#endif /* __FV_KBD_H__ */
