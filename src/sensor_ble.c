

#include "sensor_ble.h"
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>


static volatile int ble_is_advertising = 0;

int ble_setup(ble_config_t *config)
{
    int ret;
    /* Initialize the Bluetooth Subsystem */
    ret = bt_enable(NULL);
    if (ret) {
        return ret;
    }

    struct bt_le_adv_param *adv_param =
	BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE, 800, 801, NULL);

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
    if (!ble_is_advertising) {
        bt_le_adv_stop();
        ble_is_advertising = 0;
        return 0;
    }
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