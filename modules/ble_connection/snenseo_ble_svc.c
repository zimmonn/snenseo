#include "snenseo_ble_svc.h"

LOG_MODULE_REGISTER(snenseo_svc, CONFIG_SNENSEO_BLE_SVC_LOG_LEVEL);

/** @brief Static variables for indication of empty Charcteristic */
static uint8_t indicate_enabled = 0;
static struct bt_gatt_indicate_params ind_params;

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
        LOG_INF("Configuratiojn changed: %0x02", value);
	indicate_enabled = (value == BT_GATT_CCC_INDICATE);
}


static ssize_t read_snenseo_empty(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
		        	uint16_t len, uint16_t offset) {
    uint8_t empty = 0;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, (void*) &empty, 1);;
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