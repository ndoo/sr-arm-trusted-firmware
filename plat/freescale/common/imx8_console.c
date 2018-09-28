/*
 * Copyright (c) 2017-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <console.h>
#include <imx8_console.h>
#include <platform_def.h>

static console_mxcuart_t mx8_console;

void bl31_console_setup(void)
{
	/* Initialize the console to provide early debug support */
	int rc = console_mxcuart_register(IMX_BOOT_UART_BASE, IMX_BOOT_UART_CLK_IN_HZ,
					  IMX_CONSOLE_BAUDRATE, &mx8_console);

	if (rc == 0) {
		/*
		 * The crash console doesn't use the multi console API, it uses
		 * the core console functions directly. It is safe to call panic
		 * and let it print debug information.
		 */
		panic();
	}

	console_set_scope(&mx8_console.console,
			  CONSOLE_FLAG_BOOT | CONSOLE_FLAG_RUNTIME);
}
