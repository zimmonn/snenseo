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

/** @brief Snenseo turn on Characteristic UUID */
#define BT_UUID_SNENSEO_TURN_ON_VAL \
    BT_UUID_128_ENCODE(BT_UID_SNENSEO_BASE+3, 0x82e2, 0x4279, 0x939f, 0x619a6607319c)
#define BT_UUID_SNENSEO_TURN_ON BT_UUID_DECLARE_128(BT_UUID_SNENSEO_TURN_ON_VAL)

BUILD_ASSERT(sizeof(int) >= sizeof(int32_t),
             "RTC driver uses int smaller than snenseo_rtc_time fields! Problems could occur :(");

/**
 * @brief Enum for weekday bits
 */
typedef enum {
	SUN = 1,
	MON = 2,
	TUE = 4,
	WEN = 8,
	THR = 16,
	FRI = 32, 
	SAT = 64  
} weekdays_t;

/**
 * @brief Structure for storing date and time values with sub-second precision.
 *
 * @details The structure is 1-1 mapped to the struct tm for the members
 * \p tm_sec to \p tm_isdst making it compatible with the standard time library.
 *
 * @note This is just a copy from @ref drivers/rtc.h
 */
struct snenseo_rtc_time {
	int32_t tm_sec;	/**< Seconds [0, 59] */
	int32_t tm_min;	/**< Minutes [0, 59] */
	int32_t tm_hour;	/**< Hours [0, 23] */
	int32_t tm_mday;	/**< Day of the month [1, 31] */
	int32_t tm_mon;	/**< Month [0, 11] */
	int32_t tm_year;	/**< Year - 1900 */
	int32_t tm_wday;	/**< Day of the week [0, 6] (Sunday = 0) (Unknown = -1) */
	int32_t tm_yday;	/**< Day of the year [0, 365] (Unknown = -1) */
	int32_t tm_isdst;	/**< Daylight saving time flag [-1] (Unknown = -1) */
	int32_t tm_nsec;	/**< Nanoseconds [0, 999999999] (Unknown = 0) */
};

/** @brief Snenseo set time Characteristic length */
#define SNENSEO_SET_TIME_CHAR_LEN sizeof(struct snenseo_rtc_time)

/** @brief Snenseo write set time characteristic handler function pointer signature*/
typedef void (* snenseo_write_set_time_handler_t) ( struct snenseo_rtc_time* ); 

/** @brief Snenseo write turn on characteristic handler function pointer signature*/
typedef void (* snenseo_write_trun_on_handler_t) ( void ); 

/**
 * @brief Register Snenseo set time handler
 * 
 * @note If called twice first handler is overwritten without mercy. Register Null for unregister.
 * 
 * @param handler Handler to be called when characteristic is being written by GATT client  
 */
void snenseo_register_set_time_handler(snenseo_write_set_time_handler_t handler);

/**
 * @brief Register Snenseo turn on handler
 * 
 * @note If called twice first handler is overwritten without mercy. Register Null for unregister.
 * 
 * @param handler Handler to be called when characteristic is being written by GATT client  
 */
void snenseo_register_turn_on_handler(snenseo_write_trun_on_handler_t handler);

#ifdef __cplusplus
}
#endif

#endif  /* SNENSEO_BLE_SVC_H */
