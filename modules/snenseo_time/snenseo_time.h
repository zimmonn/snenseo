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

#ifdef CONFIG_BOARD_NATIVE_SIM
    #include "native_rtc.h"
#endif
#include <zephyr/drivers/rtc.h>
#include "snenseo_ble_svc.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WAITING,
    ON_DATE,
    EXPIRED
} time_state_t;

/**
 * @brief struct for timer tasks
 * 
 *  
 */
typedef struct timer_task{
    struct timer_task *next;
    struct snenseo_rtc_time trig_tim;
    uint8_t week_days;
    struct k_work* tt_work;
} timer_task_t;

/**
 * @brief Initialized the Snenseo time module. 
 */
int snenseo_init_time(void);

/**
 * @brief handler function to set date and time
 */
void snenseo_time_set_date_time(struct snenseo_rtc_time* tm);

/**
 * @brief Setting a timer task for snenseo 
 */
void snenseo_set_timer(struct snenseo_rtc_time *trig_tim, uint8_t weekdays, struct k_work* work);

#ifdef __cplusplus
}
#endif

#endif  /* SNENSEO_TIME_H */
