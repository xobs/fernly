.text

.global _start
_start:
	ldr	r0, =0x7000cffc // stack_start
	mov	sp, r0
	mov	r2, #0xffffffff
	ldr	r1, =0x7000c000 // stack_end

clear_stack:
	cmp	r1, r0
	str	r2, [r0]
	sub	r0, r0, #4
	bcc	clear_stack

	bl	wdt_disable

disable_interrupts:
	mrs	r0, cpsr
	mov	r1, #0xc0
	orr	r0, r0, r1
	msr	cpsr_cxsf, r0


//clear_psram:
//	mov r0, #0
//	mov r1, #0x00800000
//	mov	r2, #0
//clear_psram_loop:
//	cmp	r1, r0
//	str	r2, [r0], #4
//	bcc	clear_psram_loop

print_welcome_banner:
	adr	r0, welcome_banner
	bl	uart_puts

load_program:
	adr	r0, size_buffer
	mov	r1, #4
	mvn	r2, #0
	ldr	r3, =0xfff03639
	blx	r3

	ldr	r1, size_buffer

# r1 now contains the number of bytes to load.
# r0 contains the current offset to write to.
# Load bytes from the serial port into RAM.
	mov	r0, #0x70000000
	mvn	r2, #0
	ldr	r3, =0xfff03639
	blx	r3

jump_to_new_program:
	adr	r0, launch_message
	bl	uart_puts
	mov	r0, #0x70000000
	mov	pc, r0

.align	4
welcome_banner: .ascii	"Fernvale bootloader\r\nWrite four bytes of program "
		.asciz	"size, then write program data...\r\n>"
launch_message: .asciz	"Launching program...\r\n"
size_buffer:	.long	0
.align	4

uart_putc:

	push	{lr}
	sub	sp, sp, #12

	add	r3, sp, #8
	strb	r0, [r3, #-4]!
	mov	r0, r3
	mov	r1, #1
	mvn	r2, #0

	// Call usb_uart_write(char *data, int bytes, int timeout)
	ldr	r3, =0xfff03653
	blx	r3

	add	sp, sp, #12
	pop	{pc}

uart_puts:
	push	{lr}
	mov	r3, r0
	mov	r1, #0

uart_puts_count_chars_loop:
	ldrb	r2, [r3], #1
	cmp	r2, #0
	beq	uart_puts_print
	add	r1, r1, #1
	b	uart_puts_count_chars_loop
uart_puts_print:
	mvn	r2, #0

	// Call usb_uart_write(char *data, int bytes, int timeout)
	ldr	r3, =0xfff03653
	blx	r3

	// Call usb_uart_flush
	ldr	r3, =0xfff04845
	blx	r3

uart_puts_exit:
	pop	{pc}


asm_memcpy:
	mov	r3, r1
	add	r3, r3, r2

asm_memcpy_loop:
	cmp	r1, r3
	ldrcc	r2, [r1], #4
	strcc	r2, [r0], #4
	bcc	asm_memcpy_loop
	bx	lr

wdt_disable:
	ldr	r1, =0xa0030000
	mov	r0, #0x2200
	str	r0, [r1]
	bx	lr
