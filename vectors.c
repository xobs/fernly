#include "serial.h"
#include "utils.h"

/* Reset handler calls main() directly */
/*
void reset_handler(void) {
	extern int main(int argc, char **argv);
	main(1, 0);
	return;
}
*/

void undef_handler(void) {
	printf("Undefined instruction exception\n");
	return;
}

void swi_handler(void) {
	printf("SWI exception\n");
	return;
}

void prefetch_abort_handler(void) {
	printf("Prefetch abort exception\n");
	return;
}

void data_abort_handler(void) {
	printf("Data abort exception\n");
	return;
}

void reserved_handler(void) {
	printf("Handled some IRQ that shouldn't exist\n");
	return;
}

void irq_handler(void) {
	printf("Handled IRQ\n");
	return;
}

void fiq_handler(void) {
	printf("Handled FIQ\n");
	return;
}
