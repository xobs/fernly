
void reset_handler(void) {
	extern int main(int argc, char **argv);
	main(1, 0);
	return;
}

void undef_handler(void) {
	return;
}

void swi_handler(void) {
	return;
}

void prefetch_abort_handler(void) {
	return;
}

void data_abort_handler(void) {
	return;
}

void reserved_handler(void) {
	return;
}

void irq_handler(void) {
	return;
}

void fiq_handler(void) {
	return;
}
