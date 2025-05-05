

#include "sensor_ble.h"
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>


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

    // /* Start advertising */
	ret = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (ret) {
        return ret;
    }
    // ret = bt_set_name(config->adv_name);
    // if (ret) {
    //     return ret;
    // }
    return 0;
}