#include "serial.h"
#include "utils.h"

void reset_handler(void) {
	extern int main(int argc, char **argv);
	serial_puts("Reset exception\n");
	main(1, 0);
	return;
}

void undef_handler(void) {
	serial_puts("Undefined instruction exception\n");
	return;
}

void swi_handler(void) {
	serial_puts("SWI exception\n");
	return;
}

void prefetch_abort_handler(void) {
	serial_puts("Prefetch abort exception\n");
	while(1);
	return;
}

void data_abort_handler(void) {
	serial_puts("Data abort exception\n");
	while(1);
	return;
}

void reserved_handler(void) {
	serial_puts("Handled some IRQ that shouldn't exist\n");
	while(1);
	return;
}

extern void irq_dispatch(void);
void irq_handler_c(void) {
	serial_puts("Handled IRQ\n");
//	while(1);
	irq_dispatch();
	return;
}

void fiq_handler(void) {
	serial_puts("Handled FIQ\n");
//	while(1);
	return;
}
