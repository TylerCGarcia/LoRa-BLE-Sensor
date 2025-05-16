
#include "sensor_app.h"
#include "sensor_ble.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_APP, LOG_LEVEL_DBG);


#define BLE_STACKSIZE			4096
#define BLE_THREAD_PRIORITY		1
/* Dynamic Threads */
K_THREAD_STACK_DEFINE(ble_stack, BLE_STACKSIZE);
struct k_thread blethread_id;

static ble_config_t ble_config = {
	.adv_opt = BT_LE_ADV_OPT_CONN,
	.adv_name = "BLE-LoRa-Sensor",
	.adv_interval_min_ms = 500,
	.adv_interval_max_ms = 510
};



int sensor_app_init(void)
{
    return 0;
}

static void ble_thread(void *arg1, void *arg2, void *arg3)
{
    int ret;
    ret = ble_setup(&ble_config);
    LOG_INF("BLE Thread Started");
    while(1)
    {
        k_sleep(K_SECONDS(1));
    }
}

int sensor_app_ble_start(void)
{
    k_thread_create(&blethread_id, ble_stack, K_THREAD_STACK_SIZEOF(ble_stack), ble_thread, NULL, NULL, NULL, BLE_THREAD_PRIORITY, 0, K_NO_WAIT);
	k_thread_start(&blethread_id);
    return 0;
}