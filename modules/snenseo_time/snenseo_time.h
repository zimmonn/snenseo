/**
 * @file
 * 
 * @brief This modules is responsible for interacting with the Senseo machin, making it a Snenseo machine 
 */

#ifndef SNENSEO_TIME_H
#define SNENSEO_TIME_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include "snenseo_ble_svc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialized the Snenseo time module. 
 */
int snenseo_init_time(void);

/**
 * @brief handler function to set date and time
 */
void snenseo_time_set_date_time(struct snenseo_rtc_time* tm);


#ifdef __cplusplus
}
#endif

#endif  /* SNENSEO_TIME_H */
