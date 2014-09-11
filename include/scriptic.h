#ifndef __SCRIPTIC_H__
#define __SCRIPTIC_H__

#define sc_end_cmd	0
#define sc_read32_cmd	1
#define sc_write32_cmd	2
#define sc_read16_cmd	3
#define sc_write16_cmd	4
#define sc_call_cmd	5
#define sc_usleep_cmd	6

#define sc_header_size (4 * 2)
#define sc_end_size (sc_header_size + 0)
#define sc_read32_size (sc_header_size + 4 * 3)
#define sc_write32_size (sc_header_size + 4 * 3)
#define sc_read16_size (sc_header_size + 4 * 2 + 1 * 4)
#define sc_write16_size (sc_header_size + 4 * 2 + 1 * 4)
#define sc_call_size (sc_header_size + 4 * 2)
#define sc_usleep_size (sc_header_size + 4 * 1)

#ifdef __ASSEMBLY__

.macro sc_header cmd
	@cmdindex=@cmdindex+1
	.long	\cmd
	.long	0 @cmdindex
.endm

.macro sc_end
	sc_header sc_end_cmd
.endm

.macro sc_read32 match, mask, addr
	sc_header sc_read32_cmd
	.long	\match
	.long	\mask
	.long	\addr
.endm

.macro sc_write32 value, mask, addr
	sc_header sc_write32_cmd
	.long	\value
	.long	\mask
	.long	\addr
.endm

.macro sc_read16 match, mask, addr
	sc_header sc_read16_cmd
	.short	\match
	.short	\mask
	.long	\addr
.endm

.macro sc_write16 value, mask, addr
	sc_header sc_write16_cmd
	.short	\value
	.short	\mask
	.long	\addr
.endm

.macro sc_call func, arg
	sc_header sc_call_cmd
	.long	\func
	.long	\arg
.endm

.macro sc_usleep microsecs
	sc_header sc_usleep_cmd
	.long	\microsecs
.endm

.macro sc_new name, major, minor, rev
.align 4
.global \name
	@cmdindex=0	// Reset command index counter variable
\name:
1:
	.asciz	"\name"
2:
	.iflt 16 - (2b - 1b)
	.error "Name too long (16-bytes max)"
	.endif

	// Zero-pad
	.ifgt 16 - (2b - 1b)
	.zero 16 - (2b - 1b)
	.endif

	.byte	\major	// ver_major
	.byte	\minor	// ver_minor
	.short	\rev	// ver_rev
	.long	0	// command_count
	// Commands will follow in RAM
.endm

#else /* ! __ASSEMBLY__ */

#include <stdint.h>

/* Header prefixed in all commands */
struct scriptic_header {
	uint32_t	command;
	uint32_t	index;
} __attribute__((__packed__));

/* Stop execution */
struct scriptic_end {
	struct scriptic_header	header;
} __attribute__((__packed__));

/* Read 32-bit value (and optionally wait for a value) */
struct scriptic_read32 {
	struct scriptic_header	 header;
	uint32_t		 match;		/* Value to match */
	uint32_t		 mask;		/* Mask for bits to match */
	uint32_t		 addr;		/* Address to read from */
} __attribute__((__packed__));

/* Write a 32-bit value to an address */
struct scriptic_write32 {
	struct scriptic_header	 header;
	uint32_t		 value;		/* Value to write */
	uint32_t		 mask;		/* Mask for bits to write */
	uint32_t		 addr;		/* Address to write to */
} __attribute__((__packed__));

/* Read 16-bit value (and optionally wait for a value) */
struct scriptic_read16 {
	struct scriptic_header	 header;
	uint16_t		 match;		/* Value to match */
	uint16_t		 mask;		/* Mask for bits to match */
	uint32_t		 addr;		/* Address to read from */
} __attribute__((__packed__));

/* Write a 16-bit value to an address */
struct scriptic_write16 {
	struct scriptic_header	 header;
	uint16_t		 value;		/* Value to write */
	uint16_t		 mask;		/* Mask for bits to write */
	uint32_t		 addr;		/* Address to write to */
} __attribute__((__packed__));

/* Call a function repeatedly and wait for it to return true */
struct scriptic_call {
	struct scriptic_header	 header;
	int 			(*func)(void *arg);	/* Function to call */
	void			*opaque;		/* Arg to pass */
} __attribute__((__packed__));

/* Sleep for a given number of microseconds */
struct scriptic_usleep {
	struct scriptic_header	 header;
	uint32_t		 usecs;		/* Number of microseconds */
} __attribute__((__packed__));

union scriptic_command {
	struct scriptic_header	header;
	struct scriptic_end	end;
	struct scriptic_read32	read32;
	struct scriptic_write32	write32;
	struct scriptic_read16	read16;
	struct scriptic_write16	write16;
	struct scriptic_call	call;
	struct scriptic_usleep	usleep;
};

struct scriptic {
	const char 	 	name[16];
	uint8_t		 	ver_major;
	uint8_t		 	ver_minor;
	uint16_t	 	ver_rev;
	uint32_t 	 	command_count;
} __attribute__((__packed__));

int scriptic_execute(const struct scriptic *script);
const struct scriptic *scriptic_get(const char *name);
int scriptic_run(const char *name);

#endif /* __ASSEMBLY__ */

#endif /* __SCRIPTIC_H__ */
