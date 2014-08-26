#include "serial.h"
#include "printf.h"
#include "irq.h"
#include "memio.h"

static struct {
	void (*handler)(enum irq_number irq_num, void *opaque);
	void *opaque;
} handlers[__irq_max__];

int irq_init(void)
{
	register int var;

	/* Acknowledge all interrupts */
	writel(0xffffffff, IRQ_BASE + IRQ_MASK_OFF + IRQ_NUM_ADJ(0));
	writel(0xffffffff, IRQ_BASE + IRQ_MASK_OFF + IRQ_NUM_ADJ(32));

	asm volatile ("mrs %0, cpsr":"=r" (var));
	if (!(var & 0x80)) {
		serial_puts("Interrupts already enabled\n");
		return -1;
	}

	serial_puts("Interrupts were disabled.  Re-enabling...\n");
	var &= ~0x80;
	var |= 0x40;
	var &= ~0x1f;
	var |= 0x10;
	asm volatile ("msr cpsr, %0":"=r" (var));

	return 0;
}

int fiq_init(void)
{
	serial_puts("FIQs compiled out\n");
	return -1;
	/*
	register int var;
	asm volatile ("mrs %0, cpsr":"=r" (var));
	if (!(var & 0x40)) {
		serial_puts("FIQ already enabled\n");
		return -1;
	}

	serial_puts("FIQ was disabled.  Re-enabling...\n");
	var &= ~0x40;
	asm volatile ("msr cpsr, %0":"=r" (var));

	return 0;
	*/
}

int irq_enable(enum irq_number irq_num)
{
	uint32_t reg = IRQ_BASE + IRQ_MASK_OFF + IRQ_CLR + IRQ_NUM_ADJ(irq_num);
	if (irq_num >= __irq_max__)
		return -1;

	writel(1 << (irq_num & 31), reg);
	return 0;
}

int irq_disable(enum irq_number irq_num)
{
	uint32_t reg = IRQ_BASE + IRQ_MASK_OFF + IRQ_SET + IRQ_NUM_ADJ(irq_num);

	if (irq_num >= __irq_max__)
		return -1;

	writel(1 << (irq_num & 31), reg);
	return 0;
}

void irq_stimulate(enum irq_number irq_num)
{
	uint32_t reg = IRQ_BASE + IRQ_STIM_OFF + IRQ_SET + IRQ_NUM_ADJ(irq_num);
	writel(1 << (irq_num & 31), reg);
}

void irq_stimulate_reset(enum irq_number irq_num)
{
	uint32_t reg = IRQ_BASE + IRQ_STIM_OFF + IRQ_CLR + IRQ_NUM_ADJ(irq_num);
	writel(1 << (irq_num & 31), reg);
}

void irq_acknowledge(enum irq_number irq_num)
{
	uint32_t reg = IRQ_BASE + IRQ_ACK_OFF + IRQ_NUM_ADJ(irq_num);

	if (irq_num >= __irq_max__)
		return;

	writel(1 << (irq_num & 31), reg);
	return;
}

void irq_mask_acknowledge(enum irq_number irq_num)
{
	irq_disable(irq_num);
	irq_acknowledge(irq_num);
}

void irq_register_handler(enum irq_number irq_num,
                void (*handler)(enum irq_number irq_num, void *opaque),
		void *opaque)
{
	if (irq_num >= __irq_max__)
		return;
	handlers[irq_num].handler = handler;
	handlers[irq_num].opaque  = opaque;
}

static void irq_dispatch_one(enum irq_number irq_num)
{
	if (handlers[irq_num].handler)
		handlers[irq_num].handler(irq_num, handlers[irq_num].opaque);
	else
		printf("Unhandled IRQ: %d\n", irq_num);
	irq_acknowledge(irq_num);
}

void irq_dispatch(void)
{
	uint32_t reg = IRQ_BASE + IRQ_STATUS_OFF;
	uint32_t val;
	int i;

	val = readl(reg);

	for (i = 0; i < 32; i++)
		if (val & (1 << i))
			irq_dispatch_one(i);

	reg += IRQ_BASE + IRQ_STATUS_OFF + 4;
	val = readl(reg);
	for (i = 0; i < (__irq_max__ - 32); i++)
		if (val & (1 << i))
			irq_dispatch_one(32 + i);
}
