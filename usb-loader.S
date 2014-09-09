.text

.global _start
_start:
disable_interrupts:
	mrs	r0, cpsr
	mov	r1, #0xc0
	orr	r0, r0, r1
	msr	cpsr_cxsf, r0

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
