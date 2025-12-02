/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "snenseo_ble_connection.h"
#include "snenseo_ble_svc.h"
#include "snenseo_machine.h"
#ifdef CONFIG_SNENSEO_TIME
#include "snenseo_time.h"
#endif

int main(void)
{
	snenseo_init_machine();
	#ifdef CONFIG_SNENSEO_TIME
	snenseo_register_set_time_handler(snenseo_time_set_date_time);
	#endif
	snenseo_register_turn_on_handler(snenseo_machine_turn_on);
	printf("Initializing Snenseo connection\n");
	if (!snenseo_ble_connection_init()) {
		printf("Failed to initialize Snenseo connection\n");
		return 0;
	}
	printf("Initializing Snenseo connection success\n");
	return 0;
}
