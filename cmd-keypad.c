#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "printf.h"
#include "serial.h"

int cmd_keypad(int argc, char **argv)
{
	int end=0;
	char key_keys[18]="LRUDAB123456789*0#";
	uint32_t key_addr[18]={0xa00d0008,0xa00d0008,0xa00d0004,0xa00d0004,0xa00d000c,0xa00d000c,0xa00d0008,0xa00d0004,0xa00d000c,0xa00d0008,0xa00d0004,0xa00d000c,0xa00d0008,0xa00d0004,0xa00d000c,0xa00d0008,0xa00d0008,0xa00d0008};
	uint32_t key_mask[18]={0x0000fffb,0x0000fff7,0x0000fdff,0x0000fbff,0x0000ffef,0x0000ffdf,0x0000ffef,0x0000f7ff,0x0000ffbf,0x0000ffdf,0x0000efff,0x0000ff7f,0x0000ffbf,0x0000dfff,0x0000feff,0x0000dfff,0x0000bfff,0x00007fff};
	uint32_t key_state[18]={0};

	printf("Press # on keypad to exit\n");
	while(!end)
	{
		int i=0;
		for(i=0;i<18;i++)
		{
			int v=readl(key_addr[i]);
			int newstate=(v==key_mask[i]);
			if(newstate && !key_state[i])
			{
				serial_putc(key_keys[i]);
				if(i==17) end=1;
			}
			key_state[i]=newstate;		
		}
	}
	return 0;
}
