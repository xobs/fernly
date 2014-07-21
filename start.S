.section vectors

.global reset
reset:
	mov	r0, #48
	bl	uart_putc
	mov	r0, #49
	bl	uart_putc

	ldr	r0, =0x7000cffc // stack_start
	mov	sp, r0
	mov	r2, #0xffffffff
	ldr	r1, =0x7000c000 // stack_end

clear_stack:
	cmp	r1, r0
	str	r2, [r0]
	sub	r0, r0, #4
	bcc	clear_stack

	mov	r0, #66
	bl	uart_putc

# Copy 0x10000 bytes from =spi_offset to psram at 0x20000
copy_code_to_ram:
	ldr	r0, =0x00002000 // Source offset
	ldr	r1, =0x10003460 // spi_offset
	mov	r2, r1
	mov	r3, #0x10000
	add	r2, r2, r3

copy_code_to_ram_loop:
	cmp	r1, r2
	ldrcc	r3, [r1], #4
	strcc	r3, [r0], #4
	bcc	copy_code_to_ram_loop
	
# Jump to main, which ought to be in psram now
jump_to_main:
	mov	r0, #69
	bl	uart_putc

	ldr	r0, =reset_handler
	mov	pc, r0

uart_putc:
	ldr	r2, =0xa0080014 // uart offset
uart_putc_ready_wait:
	ldr	r1, [r2]
	tst	r1, #0x20
	beq	uart_putc_ready_wait
	sub	r2, r2, #0x14
	str	r0, [r2]
	bx	lr

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

.global ram_memcpy
ram_memcpy: 
	ldr	r0, =ram_memcpy_addr
	ldr	r0, [r0]
	mov	pc, r0
ram_memcpy_addr:
        .long	0x70007388

.global ram_bzero
ram_bzero:
	ldr	r0, =ram_bzero_addr
	ldr	r0, [r0]
	mov	pc, r0
ram_bzero_addr:
        .long	0x700073bc

