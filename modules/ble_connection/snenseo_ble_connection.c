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
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
        LOG_WRN("Disconnected (reason %u)\n", reason);
}

struct bt_conn_cb connection_callbacks = {
        .connected = on_connected,
        .disconnected = on_disconnected,
        .recycled = recycled_cb,
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
