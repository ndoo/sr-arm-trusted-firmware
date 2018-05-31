/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 * Copyright 2017-2018 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <arch_helpers.h>
#include <debug.h>
#include <ddrc.h>
#include <mmio.h>
#include <platform_def.h>
#include <spinlock.h>
#include <soc.h>

struct ddrc_cfg_param {
	uint32_t offset; /*reg offset */
	uint32_t val; /* config param */
};

/* lpddr4 phy init config parameters */
static struct ddrc_cfg_param umctl2_cfg[] = {
	{ .offset = DDRC_DBG1(0)   , .val = 0x00 },
	{ .offset = DDRC_PWRCTL(0) , .val = 0x00 },
	{ .offset = DDRC_MSTR(0)   , .val = 0x00 },
	{ .offset = DDRC_MSTR2(0)  , .val = 0x00 },
	{ .offset = DDRC_DERATEEN(0), .val = 0x00 },
	{ .offset = DDRC_DERATEINT(0), .val = 0x00},
	{ .offset = DDRC_RFSHTMG(0), .val = 0x00 },
	{ .offset = DDRC_INIT0(0)  , .val = 0x00 },
	{ .offset = DDRC_INIT1(0)  , .val = 0x00 },
	{ .offset = DDRC_INIT3(0)  , .val = 0x00 },
	{ .offset = DDRC_INIT4(0)  , .val = 0x00 },
	{ .offset = DDRC_INIT6(0)  , .val = 0x00 },
	{ .offset = DDRC_INIT7(0)  , .val = 0x00 },
	{ .offset = DDRC_DRAMTMG0(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG1(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG3(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG4(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG5(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG6(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG7(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG12(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG13(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG14(0), .val = 0x00 },
	{ .offset = DDRC_DRAMTMG17(0), .val = 0x00 },

	{ .offset = DDRC_ZQCTL0(0), .val = 0x00 },
	{ .offset = DDRC_ZQCTL1(0), .val = 0x00 },
	{ .offset = DDRC_ZQCTL2(0), .val = 0x00 },

	{ .offset = DDRC_DFITMG0(0), .val = 0x00 },
	{ .offset = DDRC_DFITMG1(0), .val = 0x00 },
	{ .offset = DDRC_DFIUPD0(0), .val = 0x00 },
	{ .offset = DDRC_DFIUPD1(0), .val = 0x00 },
	{ .offset = DDRC_DFIUPD2(0), .val = 0x00 },
	{ .offset = DDRC_DFIMISC(0), .val = 0x00 },
	{ .offset = DDRC_DFITMG2(0), .val = 0x00 },

	{ .offset = DDRC_DBICTL(0), .val = 0x00 },
	{ .offset = DDRC_DFIPHYMSTR(0), .val = 0x00 },

	{ .offset = DDRC_RANKCTL(0),  .val = 0x00 },
	{ .offset = DDRC_DRAMTMG2(0), .val = 0x00 },

	/* Address mapping */
	{ .offset = DDRC_ADDRMAP0(0), .val = 0x00 },
	{ .offset = DDRC_ADDRMAP1(0), .val = 0x00 },
	{ .offset = DDRC_ADDRMAP2(0), .val = 0x00 },
	{ .offset = DDRC_ADDRMAP3(0), .val = 0x00 },
	{ .offset = DDRC_ADDRMAP4(0), .val = 0x00 },
	{ .offset = DDRC_ADDRMAP5(0), .val = 0x00 },
	{ .offset = DDRC_ADDRMAP6(0), .val = 0x00 },

	/* performance tunning */
	{ .offset = DDRC_ODTCFG(0), .val = 0x00 },
	{ .offset = DDRC_ODTMAP(0), .val = 0x00 },
	{ .offset = DDRC_SCHED(0),  .val = 0x00 },
	{ .offset = DDRC_SCHED1(0), .val = 0x00 },
	{ .offset = DDRC_PERFHPR1(0), .val = 0x00 },
	{ .offset = DDRC_PERFLPR1(0), .val = 0x00 },
	{ .offset = DDRC_PERFWR1(0),  .val = 0x00 },
	{ .offset = DDRC_DBG0(0),   .val = 0x00 },
	{ .offset = DDRC_DBG1(0),   .val = 0x00 },
	{ .offset = DDRC_DBGCMD(0), .val = 0x00 },
	{ .offset = DDRC_SWCTL(0),  .val = 0x00 },
	{ .offset = DDRC_POISONCFG(0), .val = 0x00 },
	{ .offset = DDRC_PCCFG(0), .val = 0x00 },
	{ .offset = DDRC_PCFGR_0(0), .val = 0x00 },
	{ .offset = DDRC_PCFGW_0(0), .val = 0x00 },
	{ .offset = DDRC_PCTRL_0(0), .val = 0x00 },
	{ .offset = DDRC_PCFGQOS0_0(0),  .val = 0x00 },
	{ .offset = DDRC_PCFGQOS1_0(0),  .val = 0x00 },
	{ .offset = DDRC_PCFGWQOS0_0(0), .val = 0x00 },
	{ .offset = DDRC_PCFGWQOS1_0(0), .val = 0x00 },

	/* frequency point 1 */
	{ .offset = DDRC_FREQ1_DERATEEN(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DERATEINT(0), .val = 0x00 },
	{ .offset = DDRC_FREQ1_RFSHCTL0(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_RFSHTMG(0), .val = 0x00 },
	{ .offset = DDRC_FREQ1_INIT3(0),   .val = 0x00 },
	{ .offset = DDRC_FREQ1_INIT4(0),   .val = 0x00 },
	{ .offset = DDRC_FREQ1_INIT6(0),   .val = 0x00 },
	{ .offset = DDRC_FREQ1_INIT7(0),   .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG0(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG1(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG2(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG3(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG4(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG5(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG6(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG7(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG12(0), .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG13(0), .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG14(0), .val = 0x00 },
	{ .offset = DDRC_FREQ1_DRAMTMG17(0), .val = 0x00 },
	{ .offset = DDRC_FREQ1_ZQCTL0(0),  .val = 0x00 },
	{ .offset = DDRC_FREQ1_DFITMG0(0), .val = 0x00 },
	{ .offset = DDRC_FREQ1_DFITMG1(0), .val = 0x00 },
	{ .offset = DDRC_FREQ1_DFITMG2(0), .val = 0x00 }
};

void lpddr4_save_umctl2(void)
{
	int index, offset, size;

	size = sizeof(umctl2_cfg) / 8;

	for (index = 0; index < size; index++) {
		offset = umctl2_cfg[index].offset;
		umctl2_cfg[index].val = mmio_read_32(offset);
	}
}

void lpddr4_cfg_umctl2(void)
{
	int index, offset, size, val;

	size = sizeof(umctl2_cfg) / 8;

	for (index = 0; index < size; index++) {
		offset = umctl2_cfg[index].offset;
		val = umctl2_cfg[index].val;
		mmio_write_32(offset,val);
	}
}
