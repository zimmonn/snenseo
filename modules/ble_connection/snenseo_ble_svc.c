#include "snenseo_ble_svc.h"

LOG_MODULE_REGISTER(snenseo_svc, CONFIG_SNENSEO_BLE_SVC_LOG_LEVEL);

/** @brief Static variables for indication of empty Charcteristic */
static uint8_t indicate_enabled = 0;
static struct bt_gatt_indicate_params ind_params;

/** @brief static callback variables */
static snenseo_write_set_time_handler_t __handler_set_time = NULL;
static snenseo_write_trun_on_handler_t __handler_turn_on = NULL;

// This function is called when a remote device has acknowledged the indication at its host layer
static void indicate_cb(struct bt_conn *conn, struct bt_gatt_indicate_params *params, uint8_t err)
{
	LOG_DBG("Indication %s\n", err != 0U ? "fail" : "success");
}

/**
 * @brief Callback for Snenseo indications
 */
static void snenseo_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{       
        LOG_DBG("Configuratiojn changed: %0x02", value);
	indicate_enabled = (value == BT_GATT_CCC_INDICATE);
}

static ssize_t read_snenseo_empty(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
		        	uint16_t len, uint16_t offset) {
    uint8_t empty = 0;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, (void*) &empty, 1);;
}

static void sneseo_write_set_time_cb(const void *buf) {
	struct snenseo_rtc_time* time = (struct snenseo_rtc_time*) buf; 
    LOG_DBG("RTC Time: %04u-%02u-%02u %02u:%02u:%02u.%09u (wday=%u, yday=%u, isdst=%d)",
            time->tm_year - 1900,  // tm_year is years since 1900
            time->tm_mon - 1,      // tm_mon is 0-11
            time->tm_mday,
            time->tm_hour,
            time->tm_min,
            time->tm_sec,
            time->tm_nsec,
            time->tm_wday,
            time->tm_yday,
            time->tm_isdst);
	if (__handler_set_time == NULL) {
		LOG_WRN("No set time handler registered");
		return;
	}
	// TODO: Is this problematic?
	__handler_set_time(*time); 
}

static ssize_t snenseo_write_set_time(struct bt_conn *conn, const struct bt_gatt_attr *attr,
									  const void *buf, uint16_t len, uint16_t offset,
									  uint8_t flags) {
	LOG_DBG("Set Time attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);
	if (len != SNENSEO_SET_TIME_CHAR_LEN) {
  		LOG_DBG("Write Set Time: Incorrect data length");
  		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }
    if (offset != 0) {
    	LOG_DBG("Write Set Time: Incorrect data offset");
    	return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
	sneseo_write_set_time_cb(buf);
	return len;
}

static void snenseo_write_turn_on_cb(uint8_t val) {
	if (val != 1) {
		LOG_DBG("Received 0x%02x, Snenseo only turns on if 0x01 is received", val);
		return;
	} 
	if (__handler_turn_on == NULL) {
		LOG_WRN("No turn on handler registerd");
		return;
	}
	LOG_INF("Snenseo calling write turn on handler now");
	__handler_turn_on();
}

static ssize_t snenseo_write_turn_on(struct bt_conn *conn, const struct bt_gatt_attr *attr,
									  const void *buf, uint16_t len, uint16_t offset,
									  uint8_t flags) {
	LOG_DBG("Turn On attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);
	if (len != sizeof(uint8_t)) {
  		LOG_DBG("Write Turn On: Incorrect data length");
  		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }
    if (offset != 0) {
    	LOG_DBG("Write Turn On: Incorrect data offset");
    	return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
	snenseo_write_turn_on_cb(*(uint8_t*)buf);
	return len;
}

BT_GATT_SERVICE_DEFINE(snenseo_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_SNENSEO),
    BT_GATT_CHARACTERISTIC( BT_UUID_SNENSEO_EMPTY, 
    		                BT_GATT_CHRC_READ | BT_GATT_CHRC_INDICATE, 
    			            BT_GATT_PERM_READ, 
    			            read_snenseo_empty, 
    			            NULL, 
    			            NULL),
    BT_GATT_CCC(snenseo_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC( BT_UUID_SNENSEO_SET_TIME, 
    		                BT_GATT_CHRC_WRITE, 
    			            BT_GATT_PERM_WRITE, 
    			            NULL, 
    			            snenseo_write_set_time, 
    			            NULL),
	BT_GATT_CHARACTERISTIC( BT_UUID_SNENSEO_TURN_ON, 
    		                BT_GATT_CHRC_WRITE, 
    			            BT_GATT_PERM_WRITE, 
    			            NULL, 
    			            snenseo_write_turn_on, 
    			            NULL),
);

void snenseo_incicate_empty(uint8_t empty) {
	if (!indicate_enabled) {
		return;
	}
	ind_params.attr = &snenseo_svc.attrs[2];
	ind_params.func = indicate_cb; // A remote device has ACKed at its host layer (ATT ACK)
	ind_params.destroy = NULL;
	ind_params.data = &empty;
	ind_params.len = 1;
	int ret = bt_gatt_indicate(NULL, &ind_params);
        if (ret != 0) {
                LOG_ERR("Failed to send indication (err %d)", ret);
        }
}

void senseo_register_set_time_handler(snenseo_write_set_time_handler_t handler) {
	__handler_set_time = handler;
}

void snenseo_register_turn_on_handler(snenseo_write_trun_on_handler_t handler) {
	__handler_turn_on = handler;
	LOG_INF("COOL, turn on should be possible now :)");
}
