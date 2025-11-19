#include "snenseo_ble_connection.h"
#include "snenseo_ble_svc.h"

LOG_MODULE_REGISTER(snenseo_conn, CONFIG_SNENSEO_BLE_CONNECTION_LOG_LEVEL);

static const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
        (BT_LE_ADV_OPT_CONN |
         BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
        800, /* Min Advertising Interval 500ms (800*0.625ms) */
        1600, /* Max Advertising Interval 1000ms (1600*0.625ms) */
        NULL); /* Set to NULL for undirected advertising */

static const struct bt_data ad[] = {
        /* STEP 3.1 - Set the flags and populate the device name in the advertising packet */
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
        BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
        BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SNENSEO_VAL),
};

/** @brief see bt-fund l3_e2_sol STEP 10 - Define the function to update the connection's data length */
static void update_data_length(struct bt_conn *conn)
{
    int err;
    struct bt_conn_le_data_len_param my_data_len = {
        .tx_max_len = BT_GAP_DATA_LEN_MAX,
        .tx_max_time = BT_GAP_DATA_TIME_MAX,
    };
    err = bt_conn_le_data_len_update(conn, &my_data_len);
    if (err) {
        LOG_ERR("data_len_update failed (err %d)", err);
    }
}

static void exchange_func(struct bt_conn *conn, uint8_t att_err,
              struct bt_gatt_exchange_params *params)
{
    LOG_INF("MTU exchange %s", att_err == 0 ? "successful" : "failed");
    if (!att_err) {
        uint16_t payload_mtu = bt_gatt_get_mtu(conn) - 3;   // 3 bytes used for Attribute headers.
        LOG_INF("New MTU: %d bytes", payload_mtu);
    }
}

/* STEP 11.1 - Define the function to update the connection's MTU */
static void update_mtu(struct bt_conn *conn)
{
    int err;
    static struct bt_gatt_exchange_params exchange_params = {
        .func = exchange_func,
    };

    err = bt_gatt_exchange_mtu(conn, &exchange_params);
    if (err) {
        LOG_ERR("bt_gatt_exchange_mtu failed (err %d)", err);
    }
}

static void adv_work_handler(struct k_work *work)
{
        int err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));

        if (err) {
                LOG_ERR("Advertising failed to start (err %d)", err);
                return;
        }

        LOG_INF("Advertising successfully started");
}

K_WORK_DEFINE(adv_work, adv_work_handler);

static void advertising_start(void)
{
        LOG_INF("Starting to adverise Snenseo");
        k_work_submit(&adv_work);
}

static void recycled_cb(void)
{
        LOG_INF("Connection object available from previous conn. Disconnect is complete!");
        advertising_start();
}

static void on_connected(struct bt_conn *conn, uint8_t err)
{
        if (err) {
                LOG_ERR("Connection failed (err %u)\n", err);
                return;
        }

        LOG_INF("Connected\n");

        struct bt_conn_info info;
	err = bt_conn_get_info(conn, &info);
	if (err) {
		LOG_ERR("bt_conn_get_info() returned %d", err);
		return;
	}
	/* STEP 1.2 - Add the connection parameters to your log */
	double connection_interval = info.le.interval*1.25; // in ms
	uint16_t supervision_timeout = info.le.timeout*10; // in ms
	LOG_INF("Connection parameters: interval %.2f ms, latency %d intervals, timeout %d ms", connection_interval, info.le.latency, supervision_timeout);
        update_data_length(conn);
	update_mtu(conn);
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
        LOG_WRN("Disconnected (reason %u)\n", reason);
}

void on_le_data_len_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info)
{
    uint16_t tx_len     = info->tx_max_len; 
    uint16_t tx_time    = info->tx_max_time;
    uint16_t rx_len     = info->rx_max_len;
    uint16_t rx_time    = info->rx_max_time;
    LOG_INF("Data length updated. Length %d/%d bytes, time %d/%d us", tx_len, rx_len, tx_time, rx_time);
}

struct bt_conn_cb connection_callbacks = {
        .connected = on_connected,
        .disconnected = on_disconnected,
        .recycled = recycled_cb,
        .le_data_len_updated = on_le_data_len_updated,
};

int snenseo_ble_connection_init( void ) {
    int err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d)\n", err);
        return err;
    }
    err = bt_conn_cb_register(&connection_callbacks);

    if (err) {
        LOG_ERR("Registering callbacks failed (err %d)\n", err);
        return err;
    }
    LOG_INF("Bluetooth initialized");
    advertising_start();
    return err;
}
