#ifndef SNENSEO_BLE_CONNECTION_H
#define SNENSEO_BLE_CONNECTION_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME CONFIG_SNENSEO_BLE_SERVICES_DEV_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)


int snenseo_ble_connection_init( void );

#ifdef __cplusplus
}
#endif

#endif  /* SNENSEO_BLE_CONNECTION_H */