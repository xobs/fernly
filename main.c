#include "serial.h"

int main(int argc, char **argv) {
	while(1) {
		int i;
		uart_puts("Hello, world!\n");
		for (i = 0; i < 0xf000; i++);
	}
	return 0;
}
