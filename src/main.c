/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "snenseo_ble_connection.h"
#include "snenseo_machine.h"

int main(void)
{
	snenseo_init_machine();
	printf("Initializing Snenseo connection\n");
	if (!snenseo_ble_connection_init()) {
		printf("Failed to initialize Snenseo connection\n");
		return 0;
	}
	printf("Initializing Snenseo connection success\n");
	return 0;
}
