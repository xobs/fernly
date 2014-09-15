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

#endif /* __FV_KBD_H__ */
