.section vectors
	b	do_reset
	b	do_undef
	b	do_swi
	b	do_prefetch_abort
	b	do_data_abort
	b	do_reserved
	b	do_irq
	b	do_fiq

do_reset:
	b	reset_handler
do_undef:
	b	undef_handler
do_swi:
	b	swi_handler
do_prefetch_abort:
	b	prefetch_abort_handler
do_data_abort:
	b	data_abort_handler
do_reserved:
	b	reserved_handler
do_irq:
	b	irq_handler
do_fiq:
	b	fiq_handler

.text

