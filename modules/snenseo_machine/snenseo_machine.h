/**
 * @file
 * 
 * @brief This modules is responsible for interacting with the Senseo machin, making it a Snenseo machine 
 */

#ifndef SNENSEO_MACHINEC_H
#define SNENSEO_MACHINEC_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialized the Snenseo machine. 
 * 
 * Every thing hardware related directly interacting with Senseo machine. 
 * This is what makes a Senseo to a Snenseo  
 */
int snenseo_init_machine(void);

/**
 * @brief This function submits work that pushes coffee button with transistor 
 */
void snenseo_machine_turn_on(void);


#ifdef __cplusplus
}
#endif

#endif  /* SNENSEO_MACHINEC_H */
