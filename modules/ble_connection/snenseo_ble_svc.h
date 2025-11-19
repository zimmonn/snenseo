#ifndef SNENSEO_BLE_SVC_H
#define SNENSEO_BLE_SVC_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

/**  @brief Snenseo 16 bit UUID base. */ 
#define BT_UID_SNENSEO_BASE 0x00003890

/** @brief Snenseo Service UUID. */
#define BT_UUID_SNENSEO_VAL \
    BT_UUID_128_ENCODE(BT_UID_SNENSEO_BASE, 0x82e2, 0x4279, 0x939f, 0x619a6607319c)
#define BT_UUID_SNENSEO BT_UUID_DECLARE_128(BT_UUID_SNENSEO_VAL)

/** @brief Snenseo empty Characteristic UUID. */
#define BT_UUID_SNENSEO_EMPTY_VAL \
    BT_UUID_128_ENCODE(BT_UID_SNENSEO_BASE+1, 0x82e2, 0x4279, 0x939f, 0x619a6607319c)
#define BT_UUID_SNENSEO_EMPTY BT_UUID_DECLARE_128(BT_UUID_SNENSEO_EMPTY_VAL)

/** @brief Snenseo set time Characteristic UUID */
#define BT_UUID_SNENSEO_SET_TIME_VAL \
    BT_UUID_128_ENCODE(BT_UID_SNENSEO_BASE+2, 0x82e2, 0x4279, 0x939f, 0x619a6607319c)
#define BT_UUID_SNENSEO_SET_TIME BT_UUID_DECLARE_128(BT_UUID_SNENSEO_SET_TIME_VAL)

#ifdef __cplusplus
}
#endif

#endif  /* SNENSEO_BLE_SVC_H */
