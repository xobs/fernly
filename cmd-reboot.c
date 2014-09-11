#include <string.h>
#include "memio.h"
#include "printf.h"

static int wdt_reboot(void)
{
	writel(0x1209, 0xa003001c);
	return 0;
}

int cmd_reboot(int argc, char **argv)
{
	printf("Rebooting...\n");
	wdt_reboot();
	while(1);
	return 0;
}
