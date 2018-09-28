/*
 * Copyright (c) 2017-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MXC_CONSOLE_H__
#define __MXC_CONSOLE_H__

#include <console.h>

#define CONSOLE_T_MXCUART_BASE	CONSOLE_T_DRVDATA

#ifndef __ASSEMBLY__

void bl31_console_setup(void);

typedef struct {
	console_t console;
	uintptr_t base;
} console_mxcuart_t;

int console_mxcuart_register(uintptr_t baseaddr, uint32_t clock, uint32_t baud,
                           console_mxcuart_t *console);
#endif

#endif /* __MXC_CONSOLE_H__ */
