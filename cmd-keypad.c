#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"

#include "fernvale-kbd.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))
#endif

const char key_vals[] = "LRUDAB123456789*0#";

const uint32_t key_addr[]={
	KBD_MEM2,
	KBD_MEM2,
	KBD_MEM1,
	KBD_MEM1,
	KBD_MEM3,
	KBD_MEM3,
	KBD_MEM2,
	KBD_MEM1,
	KBD_MEM3,
	KBD_MEM2,
	KBD_MEM1,
	KBD_MEM3,
	KBD_MEM2,
	KBD_MEM1,
	KBD_MEM3,
	KBD_MEM2,
	KBD_MEM2,
	KBD_MEM2
};

const uint32_t key_mask[] = {
	0x0000fffb,
	0x0000fff7,
	0x0000fdff,
	0x0000fbff,
	0x0000ffef,
	0x0000ffdf,
	0x0000ffef,
	0x0000f7ff,
	0x0000ffbf,
	0x0000ffdf,
	0x0000efff,
	0x0000ff7f,
	0x0000ffbf,
	0x0000dfff,
	0x0000feff,
	0x0000dfff,
	0x0000bfff,
	0x00007fff
};

int cmd_keypad(int argc, char **argv)
{
	int end = 0;
	uint32_t key_state[18] = {0};

	printf("Press %c on keypad to exit\n",
		key_vals[ARRAY_SIZE(key_vals) - 2]);
	while (!end) {
		int key;

		for (key = 0; key < (ARRAY_SIZE(key_vals) - 1); key++) {
			int v = readl(key_addr[key]);
			int newstate = (v == key_mask[key]);

			if (newstate && !key_state[key]) {
				serial_putc(key_vals[key]);
				if (key == (ARRAY_SIZE(key_vals) - 2))
					end = 1;
			}
			key_state[key] = newstate;		
		}
	}

	printf("\n");
	return 0;
}
