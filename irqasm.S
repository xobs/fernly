#define MODE_MASK       0x0000001f /* Bits 0-4: Mode bits */
#  define USR26_MODE    0x00000000 /*   26-bit User mode */
#  define FIQ26_MODE    0x00000001 /*   26-bit FIQ mode */
#  define IRQ26_MODE    0x00000002 /*   26-bit IRQ mode */
#  define SVC26_MODE    0x00000003 /*   26-bit Supervisor mode */
#  define MODE32_BIT    0x00000010 /*   Bit 4: 32-bit mode */
#  define USR_MODE      0x00000010 /*   32-bit User mode */
#  define FIQ_MODE      0x00000011 /*   32-bit FIQ mode */
#  define IRQ_MODE      0x00000012 /*   32-bit IRQ mode */
#  define SVC_MODE      0x00000013 /*   32-bit Supervisor mode */
#  define ABT_MODE      0x00000017 /*   32-bit Abort mode */
#  define UND_MODE      0x0000001b /*   32-bit Undefined mode */
#  define SYSTEM_MODE   0x0000001f /*   32-bit System mode */
#define PSR_T_BIT       0x00000020 /* Bit 5: Thumb state */
#define PSR_F_BIT       0x00000040 /* Bit 6: FIQ disable */
#define PSR_I_BIT       0x00000080 /* Bit 7: IRQ disable */
                                   /* Bits 8-23: Reserved */
#define PSR_J_BIT       0x01000000 /* Bit 24: Jazelle state bit */
                                   /* Bits 25-26: Reserved */
#define PSR_Q_BIT       0x08000000 /* Bit 27: Sticky overflow */
#define PSR_V_BIT       0x10000000 /* Bit 28: Overflow */
#define PSR_C_BIT       0x20000000 /* Bit 29: Carry/Borrow/Extend */
#define PSR_Z_BIT       0x40000000 /* Bit 30: Zero */
#define PSR_N_BIT       0x80000000 /* Bit 31: Negative/Less than */

/* CR1 bits (CP#15 CR1) */

#define CR_M            0x00000001 /* MMU enable                          */
#define CR_A            0x00000002 /* Alignment abort enable              */
#define CR_C            0x00000004 /* Dcache enable                       */
#define CR_W            0x00000008 /* Write buffer enable                 */
#define CR_P            0x00000010 /* 32-bit exception handler            */
#define CR_D            0x00000020 /* 32-bit data address range           */
#define CR_L            0x00000040 /* Implementation defined              */
#define CR_B            0x00000080 /* Big endian                          */
#define CR_S            0x00000100 /* System MMU protection               */
#define CR_R            0x00000200 /* ROM MMU protection                  */
#define CR_F            0x00000400 /* Implementation defined              */
#define CR_Z            0x00000800 /* Implementation defined              */
#define CR_I            0x00001000 /* Icache enable                       */
#define CR_V            0x00002000 /* Vectors relocated to 0xffff0000     */
#define CR_RR           0x00004000 /* Round Robin cache replacement       */
#define CR_L4           0x00008000 /* LDR pc can set T bit                */
#define CR_DT           0x00010000
#define CR_IT           0x00040000
#define CR_ST           0x00080000
#define CR_FI           0x00200000 /* Fast interrupt (lower latency mode) */
#define CR_U            0x00400000 /* Unaligned access operation          */
#define CR_XP           0x00800000 /* Extended page tables                */
#define CR_VE           0x01000000 /* Vectored interrupts                 */

#define REG_R0              (0)
#define REG_R1              (1)
#define REG_R2              (2)
#define REG_R3              (3)
#define REG_R4              (4)
#define REG_R5              (5)
#define REG_R6              (6)
#define REG_R7              (7)
#define REG_R8              (8)
#define REG_R9              (9)
#define REG_R10             (10)
#define REG_R11             (11)
#define REG_R12             (12)
#define REG_R13             (13)
#define REG_R14             (14)
#define REG_R15             (15)
#define REG_CPSR            (16)

#define XCPTCONTEXT_REGS    (17)
#define XCPTCONTEXT_SIZE    (4 * XCPTCONTEXT_REGS)

#define REG_A1              REG_R0
#define REG_A2              REG_R1
#define REG_A3              REG_R2
#define REG_A4              REG_R3
#define REG_V1              REG_R4
#define REG_V2              REG_R5
#define REG_V3              REG_R6
#define REG_V4              REG_R7
#define REG_V5              REG_R8
#define REG_V6              REG_R9
#define REG_V7              REG_R10
#define REG_SB              REG_R9
#define REG_SL              REG_R10
#define REG_FP              REG_R11
#define REG_IP              REG_R12
#define REG_SP              REG_R13
#define REG_LR              REG_R14
#define REG_PC              REG_R15

.section data
g_irqtmp:
	.word   0               /* Saved lr */
	.word   0               /* Saved spsr */
g_undeftmp:
	.word   0               /* Saved lr */
	.word   0               /* Saved spsr */
g_aborttmp:
	.word   0               /* Saved lr */
	.word   0               /* Saved spsr */

.section text
.global irq_handler

irq_handler:
        /* On entry, we are in IRQ mode.  We are free to use
         * the IRQ mode r13 and r14.
         */
        ldr     r13, .Lirqtmp
        sub     lr, lr, #4
        str     lr, [r13]                       @ save lr_IRQ
        mrs     lr, spsr
        str     lr, [r13, #4]                   @ save spsr_IRQ

        /* Then switch back to SVC mode */

        bic     lr, lr, #MODE_MASK      /* Keep F and T bits */
        orr     lr, lr, #(SVC_MODE | PSR_I_BIT)
        msr     cpsr_c, lr              /* Switch to SVC mode */

        /* Create a context structure.  First set aside a stack frame
         * and store r0-r12 into the frame.
         */

        sub     sp, sp, #XCPTCONTEXT_SIZE
        stmia   sp, {r0-r12}            /* Save the SVC mode regs */

        /* Get the correct values of r13(sp) and r14(lr) in r1 and r2 */

        add     r1, sp, #XCPTCONTEXT_SIZE
        mov     r2, r14

        /* Get the values for r15(pc) and CPSR in r3 and r4 */

        ldr     r0, .Lirqtmp            /* Points to temp storage */
        ldmia   r0, {r3, r4}            /* Recover r1=lr_IRQ, r2=spsr_IRQ */

        add     r0, sp, #(4*REG_SP)     /* Offset to pc, cpsr storage */
        stmia   r0, {r1-r4}

        /* Then call the IRQ handler with interrupts disabled. */

        mov     fp, #0                  /* Init frame pointer */
        mov     r0, sp                  /* Get r0=xcp */

#if CONFIG_ARCH_INTERRUPTSTACK > 3
        ldr     sp, .Lirqstackbase      /* SP = interrupt stack base */
        str     r0, [sp]                /* Save the user stack pointer */
        bl      irq_handler_c             /* Call the handler */
        ldr     sp, [sp]                /* Restore the user stack pointer */
#else
        bl      irq_handler_c             /* Call the handler */

        /* Restore the CPSR, SVC mode registers and return */
.Lnoirqset:
        ldr     r0, [sp, #(4*REG_CPSR)] /* Setup the SVC mode SPSR */
        msr     spsr, r0
#endif
        ldmia   sp, {r0-r15}^           /* Return */

.Lirqtmp:
        .word   g_irqtmp
#if CONFIG_ARCH_INTERRUPTSTACK > 3
.Lirqstackbase:
        .word   g_intstackbase
#endif
        .size   irq_handler, . - irq_handler
        .align  5

