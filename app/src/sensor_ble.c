

#include "sensor_ble.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/mgmt/mcumgr/transport/smp_bt.h>

LOG_MODULE_REGISTER(SENSOR_BLE, LOG_LEVEL_INF);

// static adv_interval_t adv_interval;
static volatile int ble_is_advertising = 0;
static struct bt_conn *my_conn = NULL;

typedef struct {
    adv_interval_t adv_interval;
    char adv_name[32];
} ble_adv_params_t;

static ble_adv_params_t ble_adv_params;
static int start_advertising(void);

/**
 * @brief Calculate the advertising interval in zephyr codes.
 * 
 * @param interval_ms The interval in milliseconds.
 * @return The interval in zephyr codes.
 */
static int calculate_adv_interval(int interval_ms)
{
    return (interval_ms / 0.625);
}

static void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection error %d", err);
        return;
    }
    LOG_INF("Connected");
    my_conn = bt_conn_ref(conn);
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected. Reason %d", reason);
    bt_conn_unref(my_conn);
}

static void on_recycled(void)
{
    LOG_INF("Recycled");
    start_advertising();
}

struct bt_conn_cb connection_callbacks = {
	.connected = on_connected,
	.disconnected = on_disconnected,
	.recycled = on_recycled,
};

static int start_advertising(void)
{
    int ret;

    const struct bt_le_adv_param *adv_param = BT_LE_ADV_CONN_FAST_1;

    const struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_NO_BREDR)),
        BT_DATA(BT_DATA_NAME_COMPLETE, ble_adv_params.adv_name, strlen(ble_adv_params.adv_name)),
    };

    /* Start advertising */
    ret = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (ret) {
        return ret;
    }
    ble_is_advertising = 1;
    return 0;
}

int ble_setup(ble_config_t *config)
{

    int ret;
    /* Initialize the advertising parameters. */
    strcpy(ble_adv_params.adv_name, config->adv_name);
    ble_adv_params.adv_interval.min = calculate_adv_interval(config->adv_interval_min_ms);
    ble_adv_params.adv_interval.max = calculate_adv_interval(config->adv_interval_max_ms);

    /* Register the connection callbacks. */
    bt_conn_cb_register(&connection_callbacks);

    LOG_INF("Advertising interval code min: %d, max: %d", ble_adv_params.adv_interval.min, ble_adv_params.adv_interval.max);
    /* Initialize the Bluetooth Subsystem */
    ret = bt_enable(NULL);
    if (ret) {
        return ret;
    }
    ret = start_advertising();
    if (ret) {
        return ret;
    }
    return 0;
}

int ble_end(void)
{
    // bt_disable might stop the advertising, so we don't need to call bt_le_adv_stop()
    // if (!ble_is_advertising) {
    //     bt_le_adv_stop();
    //     ble_is_advertising = 0;
    //     return 0;
    // }
    bt_disable();
    return 0;
}

int is_ble_advertising(void)
{
    return ble_is_advertising;
}

int ble_change_name(ble_config_t *config)
{
    return bt_set_name(config->adv_name);
}

adv_interval_t get_ble_adv_interval(void)
{
    return ble_adv_params.adv_interval;
}