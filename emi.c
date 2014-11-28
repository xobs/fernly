#include <string.h>
#include "bionic.h"
#include "memio.h"

#include "fernvale-emi.h"
#include "fernvale-pmic.h"

#define PSRAM_TEST_SHOULD_INVERT (1 << 0)
#define PSRAM_TEST_SHOULD_RANDOMIZE (1 << 1)

/* Use the built-in memory self test to check calibration */
static int psram_test_run(uint32_t addr, uint32_t length, uint16_t pattern)
{
	int run;
	uint32_t test_range;
	uint32_t params;
	int tries;

	test_range  = (addr   << 8) & 0xffff0000;
	test_range |= (length >> 8) - 1;

	for (run = 0; run < 4; run++) {
		/* Params contains the "start key", plus some other flags */
		params = EMI_CTRL_MBISTB_START_KEY;

		/* On some runs, invert the pattern */
		if (run & PSRAM_TEST_SHOULD_INVERT)
			params |= EMI_CTRL_MBISTB_INVERT;

		/* On some runs, randomize the data */
		if (run & PSRAM_TEST_SHOULD_RANDOMIZE)
			params |= EMI_CTRL_MBISTB_RANDOMIZE;

		/* Reset MBIST engine */
		writel(0, EMI_CTRL_MBISTA);

		/* Set range (start and length) */
		writel(test_range, EMI_CTRL_MBISTB);

		/* Kick off the test */
		writel(params, EMI_CTRL_MBISTA);

		/* Wait for it to finish */
		for (tries = 0; tries < 256; tries++)
			if (readl(EMI_CTRL_MBISTD) & EMI_CTRL_MBISTD_FINISHED)
				break;

		/* If it didn't finis, try again */
		if (!(readl(EMI_CTRL_MBISTD) & EMI_CTRL_MBISTD_FINISHED))
			continue;

		/* If the test failed, return false */
		if ( readl(EMI_CTRL_MBISTD) & EMI_CTRL_MBISTD_FAILURE )
			return 0;
	}

	return 1;
}

static int psram_test(void)
{
	int i;
	uint32_t test_start  = 0x100000;
	uint32_t test_length = 0x8000;
	
	uint16_t patterns[] = {0xffff, 0xa55a};


	for (i = 0; i < sizeof(patterns)/sizeof(*patterns); i++)
		if (!psram_test_run(test_start, test_length, patterns[i]))
			return 0;

	/* All tests passed */
	return 1;
}

/* Set all 16 DQY delays to the same value */
static void psram_set_ganged_dqy(uint8_t delay)
{
	uint32_t vals = (delay << 0)  |
			(delay << 8)  |
			(delay << 16) |
			(delay << 24);
	writel(vals, EMI_CTRL_IDLA);
	writel(vals, EMI_CTRL_IDLB);
	writel(vals, EMI_CTRL_IDLC);
	writel(vals, EMI_CTRL_IDLD);
}

static void psram_set_ganged_dqs(uint8_t delay)
{
	uint32_t vals = (delay << 24) |
			(delay << 16);
	writel(vals, EMI_CTRL_IDLE);
}

int calibrate_psram(void)
{
	int dqy_delay;
	int dqy_delay_upper = -1;
	int dqy_delay_lower = -1;

	int dqs_delay;
	int dqs_delay_upper = -1;
	int dqs_delay_lower = -1;

	for (dqy_delay = 0; dqy_delay < 32; dqy_delay++) {

		/*
		 * Since this chip is probably routed properly, assume
		 * all trace lengths require the same delay.
		 */
		psram_set_ganged_dqy(dqy_delay);
		if (psram_test()) {
			if (dqy_delay_lower == -1)
				dqy_delay_lower = dqy_delay;
			else
				dqy_delay_upper = dqy_delay;
		}
	}

	if (-1 == dqy_delay_upper)
		dqy_delay_upper = dqy_delay_lower;

	if (dqy_delay_upper != -1) {
		dqy_delay = (dqy_delay_lower + dqy_delay_upper) / 2;
		psram_set_ganged_dqy(dqy_delay);
		return 1;
	}

	/* If DQY delays fail, try DQS instead */
	psram_set_ganged_dqy(0);
	for (dqs_delay = 1; dqs_delay <= 31; dqs_delay++) {
		psram_set_ganged_dqs(dqs_delay);
		if (psram_test()) {
			if (dqs_delay_lower == -1)
				dqs_delay_lower = dqs_delay;
			else
				dqs_delay_upper = dqs_delay;
			return 1;
		}
	}

	if (-1 == dqs_delay_upper)
		dqs_delay_upper = dqs_delay_lower;

	if (dqs_delay_upper != -1) {
		dqs_delay = (dqs_delay_upper + dqs_delay_lower) / 2;
		psram_set_ganged_dqs(dqs_delay);
		return 1;
	}

	/* Failure */
	return 0;
}
