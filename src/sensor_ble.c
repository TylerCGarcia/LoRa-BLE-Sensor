

#include "sensor_ble.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>

LOG_MODULE_REGISTER(SENSOR_BLE, LOG_LEVEL_INF);

static adv_interval_t adv_interval;

static volatile int ble_is_advertising = 0;

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

int ble_setup(ble_config_t *config)
{
    int ret;
    adv_interval.min = calculate_adv_interval(config->adv_interval_min_ms);
    adv_interval.max = calculate_adv_interval(config->adv_interval_max_ms);
    LOG_INF("Advertising interval code min: %d, max: %d", adv_interval.min, adv_interval.max);
    /* Initialize the Bluetooth Subsystem */
    ret = bt_enable(NULL);
    if (ret) {
        return ret;
    }

    struct bt_le_adv_param *adv_param =
	BT_LE_ADV_PARAM(config->adv_opt, adv_interval.min, adv_interval.max, NULL);

    const struct bt_data ad[] = {
	    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
        BT_DATA(BT_DATA_NAME_COMPLETE, config->adv_name, strlen(config->adv_name)),
    };

    /* Start advertising */
	ret = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (ret) {
        return ret;
    }
    ble_is_advertising = 1;
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
    return adv_interval;
}