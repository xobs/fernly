#include "scriptic.h"
#include "fernvale-gpio.h"

  sc_new "set_kbd", 1, 0, 0
  /* Setting the pins to use as Keypad instead of GPIO */
  sc_write32 0x11100000, 0x11100000, GPIO_CTRL_MODE0
  sc_write32 0x11111011, 0x11111011, GPIO_CTRL_MODE1

  /* The keyboard controller itself is setup correctly by default, no initialisation necessary */

  sc_end
