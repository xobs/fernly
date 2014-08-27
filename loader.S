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

	adr	r0, welcome_banner
	bl	uart_puts

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
	stmfd	sp!, {lr}
	ldr	r2, =0xa0080014 // uart offset
uart_putc_ready_wait:
	ldrb	r1, [r2]
	tst	r1, #0x20
	beq	uart_putc_ready_wait

	sub	r2, r2, #0x14
	strb	r0, [r2]
	ldmfd	sp!, {pc}

uart_puts:
	stmfd	sp!, {lr}
	mov	r3, r0

uart_puts_loop:
	ldrb	r0, [r3], #1
	cmp	r0, #0
	beq	uart_exit
	bl	uart_putc
	b	uart_puts_loop
uart_exit:
	ldmfd	sp!, {pc}

uart_getc:
	stmfd	sp!, {lr}
	ldr	r2, =0xa0080014 // uart offset
uart_getc_ready_wait:
	ldrb	r1, [r2]
	tst	r1, #0x01
	beq	uart_getc_ready_wait

	sub	r2, r2, #0x14
	ldrb	r0, [r2]
	ldmfd	sp!, {pc}

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
