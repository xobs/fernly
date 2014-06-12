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

.global other
other:
	add r1, r2, r1
	bx lr

.global __start
__start:
	add r3, r2, r3
	bx lr

.global _start
_start:
	add r4, r3, r4
	bx lr

.global other2
other2:
	add r5, r4, r5
	bx lr

