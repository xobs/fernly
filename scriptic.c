#include <stdio.h>
#include "scriptic.h"
#include "bionic.h"
#include "memio.h"

//#define SCRIPTIC_DEBUG /* Enable this to print commands as they're executed */

extern struct scriptic set_plls;
extern struct scriptic enable_psram;
extern struct scriptic spi_run;
extern struct scriptic spi_run;
extern struct scriptic spi_init;

static struct scriptic *scripts[] = {
	&set_plls,
	&enable_psram,
	&spi_run,
	&spi_init,
};

#ifdef SCRIPTIC_DEBUG
static const char *command_names[] = {
	"end",
	"read32",
	"write32",
	"read16",
	"write16",
	"call",
	"usleep",
};

static void sc_print_header(void *p)
{
	union scriptic_command *cmd = p;

	printf("> %s command", command_names[cmd->header.command]);
	switch(cmd->header.command) {
	case sc_end_cmd:
		break;

	case sc_read32_cmd:
		if (cmd->read32.mask == 0 || cmd->read32.mask == 0xffffffff)
			printf(" read32 @ 0x%08x", cmd->read32.addr);
		else
			printf(" read32 @ 0x%08x, mask 0x%08x, match 0x%008x",
				cmd->read32.addr,
				cmd->read32.mask,
				cmd->read32.match);
		break;

	case sc_read16_cmd:
		if (cmd->read16.mask == 0 || cmd->read16.mask == 0xffff)
			printf(" read16 @ 0x%08x", cmd->read16.addr);
		else
			printf(" read16 @ 0x%08x, mask 0x%04x, match 0x%004x",
				cmd->read16.addr,
				cmd->read16.mask,
				cmd->read16.match);
		break;

	case sc_write32_cmd:
		if (cmd->write32.mask == 0 || cmd->write32.mask == 0xffffffff)
			printf(" write32 0x%08x @ 0x%08x",
					cmd->write32.value, cmd->write32.addr);
		else
			printf(" write32 0x%08x (mask 0x%08x) @ 0x%08x",
					cmd->write32.value,
					cmd->write32.mask,
					cmd->write32.addr);
		break;

	case sc_write16_cmd:
		if (cmd->write16.mask == 0 || cmd->write16.mask == 0xffff)
			printf(" write16 0x%04x @ 0x%08x",
					cmd->write16.value, cmd->write16.addr);
		else
			printf(" write16 0x%04x (mask 0x%04x) @ 0x%08x",
					cmd->write16.value,
					cmd->write16.mask,
					cmd->write16.addr);
		break;

	case sc_call_cmd:
		printf(" call function @ 0x%08x with arg 0x%08x",
				(uint32_t)cmd->call.func,
				(uint32_t)cmd->call.opaque);
		break;

	case sc_usleep_cmd:
		printf(" usleep for %d usecs", cmd->usleep.usecs);
		break;

	default:
		printf(" unrecognized command");
	}

	printf("\n");
}
#endif /* SCRIPTIC_DEBUG */

static int sc_header_command(struct scriptic_header *header)
{
	return header->command;
}

static int sc_command_size(void *header) {
	switch(sc_header_command(header)) {
	case sc_end_cmd:	return sizeof(struct scriptic_end);
	case sc_read32_cmd:	return sizeof(struct scriptic_read32);
	case sc_write32_cmd:	return sizeof(struct scriptic_write32);
	case sc_read16_cmd:	return sizeof(struct scriptic_read16);
	case sc_write16_cmd:	return sizeof(struct scriptic_write16);
	case sc_call_cmd:	return sizeof(struct scriptic_call);
	case sc_usleep_cmd:	return sizeof(struct scriptic_usleep);
	default:		return sizeof(struct scriptic_header);
	}
}

static struct scriptic_header *sc_next_command(void *header)
{
	return header + sc_command_size(header);
}

static int sc_command_count(struct scriptic *script)
{
	uint32_t count = 1;
	struct scriptic_header *header = (struct scriptic_header *)&script[1];

	for (count = 0; sc_header_command(header) != sc_end_cmd; count++) {
		header->index = count;
		header = sc_next_command(header);
	}
	header->index = count;

	return count;
}

/* Command functions */

static void sc_read32(struct scriptic_read32 *pkt)
{
	if ((pkt->mask == 0) || (pkt->mask == 0xffffffff))
		(void)readl(pkt->addr);
	else {
		while (1) {
			uint32_t val = readl(pkt->addr);
			if ((val & pkt->mask) == (pkt->match & pkt->mask) )
				break;
		}
	}
}

static void sc_read16(struct scriptic_read16 *pkt)
{
	if ((pkt->mask == 0) || (pkt->mask == 0xffff))
		(void)readw(pkt->addr);
	else
		while (1) {
			uint16_t val = readw(pkt->addr);
			if ((val & pkt->mask) == (pkt->match & pkt->mask) )
				break;
		}
}

static void sc_write32(struct scriptic_write32 *pkt)
{
	if ((pkt->mask == 0) || (pkt->mask == 0xffffffff)) {
		writel(pkt->value, pkt->addr);
	}
	else {
		uint32_t tmp;
		tmp = readl(pkt->addr);
		tmp &= ~pkt->mask;
		tmp |= (pkt->value & pkt->mask);
		writel(tmp, pkt->addr);
	}
}

static void sc_write16(struct scriptic_write16 *pkt)
{
	if ((pkt->mask == 0) || (pkt->mask == 0xffff)) {
		writew(pkt->value, pkt->addr);
	}
	else {
		uint16_t tmp;
		tmp = readw(pkt->addr);
		tmp &= ~pkt->mask;
		tmp |= (pkt->value & pkt->mask);
		writew(tmp, pkt->addr);
	}
}

static void sc_call(struct scriptic_call *pkt)
{
	while (!pkt->func(pkt->opaque));
}

void sc_usleep(struct scriptic_usleep *pkt)
{
	uint32_t usecs;
	int i, j;

	usecs = pkt->usecs;

	/* Outer loop is 11 cycles total, 6 cycles on its own */
	for (i = 0; i < usecs; i++)
		/* Inner loop is 5 cycles */
		for (j = 0; j < 73; j++)
			asm("nop");
}

/* Exported functions */

int scriptic_execute(const struct scriptic *script)
{
	void *header;

	if (!script) {
#ifdef SCRIPTIC_DEBUG
		printf("scriptic: Tried to execute a NULL script\n");
#endif
		return -1;
	}

	header = (struct scriptic_header *)&script[1];

#ifdef SCRIPTIC_DEBUG
	printf("Executing script \"%s\", v %d.%d.%d\n",
			script->name,
			script->ver_major, script->ver_minor, script->ver_rev);
	sc_print_header(header);
#endif /* SCRIPTIC_DEBUG */
	while (sc_header_command(header) != sc_end_cmd) {
		switch(sc_header_command(header)) {
		case sc_end_cmd:
			break;

		case sc_read32_cmd:
			sc_read32(header);
			break;

		case sc_write32_cmd:
			sc_write32(header);
			break;

		case sc_read16_cmd:
			sc_read16(header);
			break;

		case sc_write16_cmd:
			sc_write16(header);
			break;

		case sc_call_cmd:
			sc_call(header);
			break;

		case sc_usleep_cmd:
			sc_usleep(header);
			break;

		default:
			break;
		}

		header = sc_next_command(header);
#ifdef SCRIPTIC_DEBUG
		sc_print_header(header);
#endif /* SCRIPTIC_DEBUG */
	}

	return 0;
}

const struct scriptic *scriptic_get(const char *name)
{
	struct scriptic *script = NULL;
	int i;

	for (i = 0; i < sizeof(scripts) / sizeof(*scripts); i++) {
		if (!_strcasecmp(name, scripts[i]->name)) {
			script = scripts[i];
			break;
		}
	}

	if (script && script->command_count == 0)
		script->command_count = sc_command_count(script);

	return script;
}

int scriptic_run(const char *name)
{
	const struct scriptic *script;

	script = scriptic_get(name);
	if (!script) {
#ifdef SCRIPTIC_DEBUG
		printf("scriptic: Unrecognized script name: %s\n", name);
#endif
		return -1;
	}
	return scriptic_execute(script);
}
