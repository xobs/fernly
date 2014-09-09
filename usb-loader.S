.text

.global _start
_start:
//	ldr	r0, =0x7000cffc // stack_start
//	mov	sp, r0
//	mov	r2, #0xffffffff
//	ldr	r1, =0x7000c000 // stack_end
//
//clear_stack:
//	cmp	r1, r0
//	str	r2, [r0]
//	sub	r0, r0, #4
//	bcc	clear_stack
//
//	bl	wdt_disable
//
//clear_psram:
//	mov r0, #0
//	mov r1, #0x00800000
//	mov	r2, #0
//clear_psram_loop:
//	cmp	r1, r0
//	str	r2, [r0], #4
//	bcc	clear_psram_loop

//print_welcome_banner:
//	adr	r0, welcome_banner
//	bl	uart_puts

main_loop:
	adr	r0, four_byte_buffer
	mov	r1, #1
	mvn	r2, #0
	ldr	r3, =0xfff03639
	blx	r3

	adr	r0, four_byte_buffer
	mov	r1, #1
	mvn	r2, #0
	ldr	r3, =0xfff03653
	blx	r3

	ldr	r3, =0xfff04845
	blx	r3

	b	main_loop

.align	4
four_byte_buffer:
.ascii	"1234"

load_program:
	bl	uart_getc
	mov	r5, r0
	lsls	r5, r5, #0
	mov	r4, r5

	bl	uart_getc
	mov	r5, r0
	lsls	r5, r5, #8
	orr	r4, r5

	bl	uart_getc
	mov	r5, r0
	lsls	r5, r5, #16
	orr	r4, r5

	bl	uart_getc
	mov	r5, r0
	lsls	r5, r5, #24
	orr	r4, r5

# r4 now contains the number of bytes to load.
# r5 contains the current offset to write to.
# Load bytes from the serial port into RAM.
	mov	r5, #0
loader_loop:
	bl	uart_getc
	strb	r0, [r5], #1
	sub	r4, #1
	cmp	r4, #0
	bne	loader_loop

jump_to_new_program:
	adr	r0, launch_message
	bl	uart_puts
	mov	r0, #0
	mov	pc, r0

.align	4
welcome_banner:
.ascii	"Fernvale bootloader\r\nWrite four bytes of program size, then\r\n"
.asciz	"write program data...\r\n>"
launch_message:
.asciz	"Launching program...\r\n"
.align	4

uart_putc:

	push	{lr}
	sub	sp, sp, #12

	add	r3, sp, #8
	strb	r0, [r3, #-4]!
	mov	r0, r3
	mov	r1, #1
	mvn	r2, #0

	ldr	r3, =0x70004560
	ldr	r3, [r3]
	blx	r3

	add	sp, sp, #12
	pop	{pc}

uart_puts:
	push	{lr}
	mov	r3, r0
	mov	r2, r0
	mov	r1, #0

uart_puts_count_chars_loop:
	ldrb	r0, [r3], #1
	cmp	r0, #0
	beq	uart_puts_print
	add	r1, r1, #1
	b	uart_puts_count_chars_loop
uart_puts_print:
	mov	r0, r2
	mvn	r2, #0

	// Call usb_uart_write(char *data, int bytes, int timeout)
	ldr	r3, =0x70004560
	ldr	r3, [r3]
	blx	r3

	ldr	r3, =0x70004564
	ldr	r3, [r3]
	cmp	r3, #0
	beq	uart_puts_exit
	blx	r3
uart_puts_exit:
	pop	{pc}

uart_getc:
	bx	=0xfff048ba+1

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
