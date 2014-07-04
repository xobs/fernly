.section vectors
.global rv_start
rv_start:
	b	do_reset
	b	do_undef
	b	do_swi
	b	do_prefetch_abort
	b	do_data_abort
	b	do_reserved
	b	do_irq
	b	do_fiq

do_reset:
	ldr	r0, =reset_handler
	mov	pc, r0

do_undef:
	ldr	r0, =undef_handler
	mov	pc, r0

do_swi:
	ldr	r0, =swi_handler
	mov	pc, r0

do_prefetch_abort:
	ldr	r0, =prefetch_abort_handler
	mov	pc, r0

do_data_abort:
	ldr	r0, =data_abort_handler
	mov	pc, r0

do_reserved:
	ldr	r0, =reserved_handler
	mov	pc, r0

do_irq:
	ldr	r0, =irq_handler
	mov	pc, r0

do_fiq:
	ldr	r0, =fiq_handler
	mov	pc, r0

.global rv_end
rv_end:

.text


.global __udiv64

__udiv64:
	adds	r0,r0,r0
	adc	r1,r1,r1

	.rept 31
		cmp	r1,r2
		subcs	r1,r1,r2
		adcs	r0,r0,r0
		adc	r1,r1,r1
	.endr

	cmp	r1,r2
	subcs	r1,r1,r2
	adcs	r0,r0,r0

	bx	lr
