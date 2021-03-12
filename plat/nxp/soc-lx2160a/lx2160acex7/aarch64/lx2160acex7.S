/*
 * Copyright 2018-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

.section .text, "ax"

#include <asm_macros.S>

#include <nxp_timer.h>
#include <plat_gic.h>
#include <pmu.h>

#include <bl31_data.h>
#include <plat_psci.h>
#include <platform_def.h>

.global _soc_sys_off
.global init_tzpc

#define NXP_GPIO3_PD_OFFSET 0x0
#define NXP_GPIO3_PD_GPIO (1 << 24)

.equ TZPC_BASE,		     0x02200000
.equ TZPCDECPROT_0_SET_BASE, 0x02200804
.equ TZPCDECPROT_1_SET_BASE, 0x02200810
.equ TZPCDECPROT_2_SET_BASE, 0x0220081C


#if 0
/* Function sets the security mechanisms in the SoC to implement the
 * Platform Security Policy
 */
func _set_platform_security
	mov  x8, x30

#if (!SUPPRESS_TZC)
	/* initialize the tzpc */
	bl   init_cex7_tzpc
#endif

#if (!SUPPRESS_SEC)
	/* initialize secmon */
#ifdef NXP_SNVS_ENABLED
	mov x0, #NXP_SNVS_ADDR
	bl  init_sec_mon
#endif
#endif

	mov  x30, x8
	ret
endfunc _set_platform_security
#endif

/* Part of SYSTEM_OFF:
 * Function turns off the SoC clocks
 * Note: Function is not intended to return, and the only allowable
 *         recovery is POR
 * in:  none
 * out: none
 * uses x0, x1, x2, x3
 */
func _soc_sys_off

	mov x3, #NXP_GPIO3_ADDR
	ldr x0, =NXP_GPIO3_PD_OFFSET
	ldr w1, =NXP_GPIO3_PD_GPIO
	str w1, [x3, x0]

	/* A-009810: LPM20 entry sequence might cause
	 * spurious timeout reset request
	 * workaround: MASK RESET REQ RPTOE
	 */
	ldr  x0, =NXP_RESET_ADDR
	ldr  w1, =RSTRQMR_RPTOE_MASK
	str  w1, [x0, #RST_RSTRQMR1_OFFSET]

	/* disable sec, QBman, spi and qspi */
	ldr  x2, =NXP_DCFG_ADDR
	ldr  x0, =DCFG_DEVDISR1_OFFSET
	ldr  w1, =DCFG_DEVDISR1_SEC
	str  w1, [x2, x0]
	ldr  x0, =DCFG_DEVDISR3_OFFSET
	ldr  w1, =DCFG_DEVDISR3_QBMAIN
	str  w1, [x2, x0]
	ldr  x0, =DCFG_DEVDISR4_OFFSET
	ldr  w1, =DCFG_DEVDISR4_SPI_QSPI
	str  w1, [x2, x0]

	/* set TPMWAKEMR0 */
	ldr  x0, =TPMWAKEMR0_ADDR
	mov  w1, #0x1
	str  w1, [x0]

	/* disable icache, dcache, mmu @ EL1 */
	mov  x1, #SCTLR_I_C_M_MASK
	mrs  x0, sctlr_el1
	bic  x0, x0, x1
	msr  sctlr_el1, x0

	/* disable L2 prefetches */
	mrs  x0, CORTEX_A72_ECTLR_EL1
	bic  x1, x1, #CPUECTLR_TIMER_MASK
	orr  x0, x0, #CPUECTLR_SMPEN_EN
	orr  x0, x0, #CPUECTLR_TIMER_8TICKS
	msr  CORTEX_A72_ECTLR_EL1, x0
	isb

	/* disable CCN snoop domain */
	mov  x1, #NXP_CCN_HN_F_0_ADDR
	ldr  x0, =CCN_HN_F_SNP_DMN_CTL_MASK
	str  x0, [x1, #CCN_HN_F_SNP_DMN_CTL_CLR_OFFSET]
3:
	ldr  w2, [x1, #CCN_HN_F_SNP_DMN_CTL_OFFSET]
	cmp  w2, #0x2
	b.ne 3b

	mov  x3, #NXP_PMU_ADDR

4:
	ldr  w1, [x3, #PMU_PCPW20SR_OFFSET]
	cmp  w1, #PMU_IDLE_CORE_MASK
	b.ne 4b

	mov  w1, #PMU_IDLE_CLUSTER_MASK
	str  w1, [x3, #PMU_CLAINACTSETR_OFFSET]

1:
	ldr  w1, [x3, #PMU_PCPW20SR_OFFSET]
	cmp  w1, #PMU_IDLE_CORE_MASK
	b.ne 1b

	mov  w1, #PMU_FLUSH_CLUSTER_MASK
	str  w1, [x3, #PMU_CLL2FLUSHSETR_OFFSET]

2:
	ldr  w1, [x3, #PMU_CLL2FLUSHSR_OFFSET]
	cmp  w1, #PMU_FLUSH_CLUSTER_MASK
	b.ne 2b

	mov  w1, #PMU_FLUSH_CLUSTER_MASK
	str  w1, [x3, #PMU_CLSL2FLUSHCLRR_OFFSET]

	mov  w1, #PMU_FLUSH_CLUSTER_MASK
	str  w1, [x3, #PMU_CLSINACTSETR_OFFSET]

	mov  x2, #DAIF_SET_MASK
	mrs  x1, spsr_el1
	orr  x1, x1, x2
	msr  spsr_el1, x1

	mrs  x1, spsr_el2
	orr  x1, x1, x2
	msr  spsr_el2, x1

	/* force the debug interface to be quiescent */
	mrs  x0, osdlr_el1
	orr  x0, x0, #0x1
	msr  osdlr_el1, x0

	/* invalidate all TLB entries at all 3 exception levels */
	tlbi alle1
	tlbi alle2
	tlbi alle3

	/* x3 = pmu base addr */

	/* request lpm20 */
	ldr  x0, =PMU_POWMGTCSR_OFFSET
	ldr  w1, =PMU_POWMGTCSR_VAL
	str  w1, [x3, x0]

5:
	wfe
	b.eq  5b
endfunc _soc_sys_off

/* Function setc up the TrustZone Address Space Controller (TZASC)
 * in:  none
 * out: none
 * uses x0, x1
 */
func init_tzpc

	/* set Non Secure access for all devices protected via TZPC */

	/* decode Protection-0 Set Reg */
	ldr	x1, =TZPCDECPROT_0_SET_BASE
	/* set decode region to NS, Bits[7:0] */
	mov	w0, #0xFF
	str	w0, [x1]

	/* decode Protection-1 Set Reg */
	ldr	x1, =TZPCDECPROT_1_SET_BASE
	/* set decode region to NS, Bits[7:0] */
	mov	w0, #0xFF
	str	w0, [x1]

	/* decode Protection-2 Set Reg */
	ldr	x1, =TZPCDECPROT_2_SET_BASE
	/* set decode region to NS, Bits[7:0] */
#ifdef SECURE_BOOT
	mov	w0, #0xEF
#else
	mov	w0, #0xFF
#endif
	str	w0, [x1]

	/* entire SRAM as NS */
	/* secure RAM region size Reg */
	ldr	x1, =TZPC_BASE
	/* 0x00000000 = no secure region */
	mov	w0, #0x00000000
	str	w0, [x1]

	ret
endfunc init_tzpc
