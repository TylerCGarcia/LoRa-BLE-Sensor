
#include "sensor_app.h"
#include "sensor_ble.h"
#include "sensor_scheduling.h"
#include "sensor_nvs.h"
// #include "sensor_data.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(SENSOR_APP, LOG_LEVEL_DBG);

#define BLE_STACKSIZE			1024
#define BLE_THREAD_PRIORITY		1
/* Dynamic Threads */
K_THREAD_STACK_DEFINE(ble_stack, BLE_STACKSIZE);
struct k_thread blethread_id;

enum sensor_nvs_address {
	SENSOR_NVS_ADDRESS_DEV_NONCE,
	SENSOR_NVS_ADDRESS_JOIN_EUI,
	SENSOR_NVS_ADDRESS_APP_KEY,
	SENSOR_NVS_ADDRESS_DEV_EUI,
	SENSOR_NVS_ADDRESS_APP_STATE,
	SENSOR_NVS_ADDRESS_LIMIT,
};

enum sensor_timer_channel {
    SENSOR_TIMER_CHANNEL_0,
    SENSOR_TIMER_CHANNEL_1,
    SENSOR_TIMER_CHANNEL_2,
    SENSOR_TIMER_CHANNEL_LIMIT,
};

// #define DEV_EUI {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
// #define JOIN_EUI {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
// #define APP_KEY {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

// static lorawan_setup_t setup = {
// 	.lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0)),
// 	.uplink_class = LORAWAN_CLASS_A,
// 	.downlink_callback = NULL,
// 	.join_attempts = 0,
// 	.dev_nonce = 160,
// 	.delay = 1000,
// 	.dev_eui = DEV_EUI,
// 	.join_eui = JOIN_EUI,
// 	.app_key = APP_KEY,
// };

// sensor_data_t sensor1_data = {
//     .id = SENSOR_1,
//     .power_id = SENSOR_POWER_1,
//     .max_samples = 10,
//     .data_size = 4,
//     .timestamp_size = 4,
// };

// sensor_data_t sensor2_data = {
//     .id = SENSOR_2,
//     .power_id = SENSOR_POWER_2,
//     .max_samples = 10,
//     .data_size = 4,
//     .timestamp_size = 4,
// };

/* The timer device to use for scheduling */
static const struct device *sensor_timer = DEVICE_DT_GET(DT_ALIAS(sensortimer));

static ble_config_t ble_config = {
	.adv_opt = BT_LE_ADV_OPT_CONN,
	.adv_name = "BLE-LoRa-Sensor",
	.adv_interval_min_ms = 500,
	.adv_interval_max_ms = 510
};



int initialize_nvs(void)
{
    int ret;
    ret = sensor_nvs_setup(SENSOR_NVS_ADDRESS_LIMIT);
    if(ret < 0)
    {
        return ret;
    }

    /* Pull all data from NVS */

	// sensor_nvs_read(SENSOR_NVS_ADDRESS_DEV_EUI, &setup.dev_eui, sizeof(setup.dev_eui));
	// sensor_nvs_read(SENSOR_NVS_ADDRESS_JOIN_EUI, &setup.join_eui, sizeof(setup.join_eui));
	// sensor_nvs_read(SENSOR_NVS_ADDRESS_APP_KEY, &setup.app_key, sizeof(setup.app_key));
    return 0;
}

int sensor_app_init(void)
{
    int ret;
    ret = sensor_scheduling_init(sensor_timer);
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize scheduling");
        return ret;
    }
    ret = initialize_nvs();
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize NVS");
        return ret;
    }
    return 0;
}

// int sensor_app_configuration_state(void)
// {
//     return 0;
// }

// int sensor_app_running_state(void)
// {
//     return 0;
// }

// int sensor_app_error_state(void)
// {
//     return 0;
// }

static void ble_thread(void *arg1, void *arg2, void *arg3)
{
    LOG_INF("BLE Thread Started");
    int ret;
    ret = ble_setup(&ble_config);
    LOG_INF("Setting up LoRaWAN BLE Service");

    while(1)
    {
        LOG_INF("BLE Thread Running");
        k_sleep(K_SECONDS(1));
    }
}

int sensor_app_ble_start(void)
{
    k_thread_create(&blethread_id, ble_stack, K_THREAD_STACK_SIZEOF(ble_stack), ble_thread, NULL, NULL, NULL, BLE_THREAD_PRIORITY, 0, K_NO_WAIT);
	k_thread_start(&blethread_id);
    return 0;
}

// int sensor_app_ble_stop(void)
// {
//     int ret;
//     ret = ble_end();
//     LOG_INF("Stopping BLE Thread");
//     k_thread_suspend(&blethread_id);
//     return ret;
// }