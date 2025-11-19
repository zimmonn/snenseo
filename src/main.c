/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "snenseo_ble_connection.h"

int main(void)
{
	printf("Hello World! modded %s\n", CONFIG_BOARD_TARGET);
	printf("Initializing Snenseo connection\n");
	if (!snenseo_ble_connection_init()) {
		printf("Failed to initialize Snenseo connection\n");
		return 0;
	}
	printf("Initializing Snenseo connection success\n");
	return 0;
}
