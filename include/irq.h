#ifndef __IRQ_H__
#define __IRQ_H__

enum irq_number {
	irq_unk_0,
	irq_unk_1,
	irq_unk_2,
	irq_unk_3,
	irq_unk_4,
	irq_unk_5,
	irq_unk_6,
	irq_unk_7,
	irq_unk_8,
	irq_unk_9,
	irq_unk_10,
	irq_unk_11,
	irq_unk_12,
	irq_unk_13,
	irq_unk_14,
	irq_uart1,
	irq_unk_16,
	irq_uart2,
	irq_unk_18,
	irq_unk_19,
	irq_unk_20,
	irq_unk_21,
	irq_unk_22,
	irq_unk_23,
	irq_unk_24,
	irq_unk_25,
	irq_unk_26,
	irq_unk_27,
	irq_unk_28,
	irq_unk_29,
	irq_unk_30,
	irq_unk_31,
	irq_unk_32,
	irq_unk_33,
	irq_unk_34,
	irq_unk_35,
	irq_unk_36,
	irq_unk_37,
	irq_unk_38,
	irq_unk_39,
	irq_unk_40,
	irq_unk_41,
	irq_unk_42,
	irq_unk_43,
	irq_unk_44,
	irq_unk_45,
	irq_unk_46,
	irq_unk_47,
	__irq_max__,
};

int irq_init(void);
int fiq_init(void);
int irq_enable(enum irq_number irq_num);
int irq_disable(enum irq_number irq_num);
void irq_acknowledge(enum irq_number irq_num);
void irq_mask_acknowledge(enum irq_number irq_num);
void irq_dispatch(void);
void irq_stimulate(enum irq_number irq_num);
void irq_stimulate_reset(enum irq_number irq_num);

void irq_register_handler(enum irq_number irq_num,
		void (*handler)(enum irq_number irq_num, void *opaque),
		void *opaque);

#define IRQ_BASE (0xa0060000)
#define IRQ_MASK_OFF 0x00	/* IRQ mask (enabled/disabled) */
#define IRQ_SENSE_OFF 0x60	/* IRQ sensitivity (edge vs level) */
#define IRQ_STIM_OFF 0xc0	/* IRQ "stimulate" (for debug) */
#define IRQ_STATUS_OFF 0x100	/* IRQ status (IRQ firing or not) */
#define FIQ_STATUS_OFF 0x140	/* FIQ status (FIQ firing or not) */
#define IRQ_ACK_OFF 0x160	/* IRQ acknowledge (write a 1 to acknowledge) */
#define FIQ_SOURCE_OFF 0x180	/* IRQ number used as FIQ */

#define IRQ_SET 0x20		/* Offset from OFF for "SET" behaviour */
#define IRQ_CLR 0x40		/* Offset from OFF for "CLR" behaviour */

#define IRQ_NUM_ADJ(x) (((x) > 32) ? 4 : 0) /* Offset for IRQs > 32 */

#endif /* __IRQ_H__ */
