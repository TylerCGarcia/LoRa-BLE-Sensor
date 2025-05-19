
#include "sensor_app.h"
#include "sensor_ble.h"
#include "sensor_scheduling.h"
#include "sensor_data.h"
#include "sensor_nvs.h"
#include "sensor_lorawan.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(SENSOR_APP, LOG_LEVEL_DBG);

#define BLE_STACKSIZE			1024
#define BLE_THREAD_PRIORITY		1
/* Dynamic Threads */
K_THREAD_STACK_DEFINE(ble_stack, BLE_STACKSIZE);
struct k_thread blethread_id;

static sensor_app_config_t *sensor_app_config;

enum sensor_nvs_address {
    SENSOR_NVS_ADDRESS_APP_STATE,
    SENSOR_NVS_ADDRESS_LORAWAN_ENABLED,
    SENSOR_NVS_ADDRESS_LORAWAN_DEV_EUI,
	SENSOR_NVS_ADDRESS_LORAWAN_JOIN_EUI,
	SENSOR_NVS_ADDRESS_LORAWAN_APP_KEY,
    SENSOR_NVS_ADDRESS_LORAWAN_DEV_NONCE,
    SENSOR_NVS_ADDRESS_LORAWAN_JOIN_ATTEMPTS,
    SENSOR_NVS_ADDRESS_LORAWAN_SEND_ATTEMPTS,
    SENSOR_NVS_ADDRESS_SENSOR_1_ENABLED,
    SENSOR_NVS_ADDRESS_SENSOR_1_POWER,
    SENSOR_NVS_ADDRESS_SENSOR_1_TYPE,
    SENSOR_NVS_ADDRESS_SENSOR_1_FREQUENCY,
    SENSOR_NVS_ADDRESS_SENSOR_2_ENABLED,
    SENSOR_NVS_ADDRESS_SENSOR_2_POWER,
    SENSOR_NVS_ADDRESS_SENSOR_2_TYPE,
    SENSOR_NVS_ADDRESS_SENSOR_2_FREQUENCY,
	SENSOR_NVS_ADDRESS_LIMIT,
};

enum sensor_timer_channel {
    SENSOR_TIMER_CHANNEL_0,
    SENSOR_TIMER_CHANNEL_1,
    SENSOR_TIMER_CHANNEL_2,
    SENSOR_TIMER_CHANNEL_LIMIT,
};

#define DEV_EUI {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define JOIN_EUI {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define APP_KEY {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

static lorawan_setup_t lorawan_setup = {
	.lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0)),
	.uplink_class = LORAWAN_CLASS_A,
	.downlink_callback = NULL,
	.join_attempts = 0,
	.dev_nonce = 160,
	.delay = 1000,
	.dev_eui = DEV_EUI,
	.join_eui = JOIN_EUI,
	.app_key = APP_KEY,
	.send_attempts = 0,
};

static sensor_data_t sensor1_data = {
    .id = SENSOR_1,
    .power_id = SENSOR_POWER_1,
    .max_samples = 10,
    .data_size = 4,
    .timestamp_size = 4,
};

static sensor_data_t sensor2_data = {
    .id = SENSOR_2,
    .power_id = SENSOR_POWER_2,
    .max_samples = 10,
    .data_size = 4,
    .timestamp_size = 4,
};

/* The timer device to use for scheduling */
static const struct device *sensor_timer = DEVICE_DT_GET(DT_ALIAS(sensortimer));

static ble_config_t ble_config = {
	.adv_opt = BT_LE_ADV_OPT_CONN,
	.adv_name = "BLE-LoRa-Sensor",
	.adv_interval_min_ms = 500,
	.adv_interval_max_ms = 510
};

static int initialize_nvs_address(enum sensor_nvs_address address, void *data, size_t size)
{
    int ret;
    ret = sensor_nvs_read(address, data, size);
    if(ret < 0)
    {
        LOG_INF("No data found in NVS address %d", address);
        if(sensor_nvs_write(address, data, size) < 0)
        {
            LOG_ERR("Failed to write data to NVS address %d", address);
            return ret;
        }
    }
    return 0;
}

static int initialize_lorawan_nvs(void)
{
    LOG_INF("Reading LoRaWAN NVS");
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_ENABLED, &sensor_app_config->is_lorawan_enabled, sizeof(sensor_app_config->is_lorawan_enabled));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_DEV_EUI, &lorawan_setup.dev_eui, sizeof(lorawan_setup.dev_eui));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_JOIN_EUI, &lorawan_setup.join_eui, sizeof(lorawan_setup.join_eui));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_APP_KEY, &lorawan_setup.app_key, sizeof(lorawan_setup.app_key));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_DEV_NONCE, &lorawan_setup.dev_nonce, sizeof(lorawan_setup.dev_nonce));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_JOIN_ATTEMPTS, &lorawan_setup.join_attempts, sizeof(lorawan_setup.join_attempts));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_SEND_ATTEMPTS, &lorawan_setup.send_attempts, sizeof(lorawan_setup.send_attempts));
    return 0;
}

static int initialize_sensor_nvs(void)
{
    LOG_INF("Reading Sensor NVS");
    initialize_nvs_address(SENSOR_NVS_ADDRESS_APP_STATE, &sensor_app_config->state, sizeof(sensor_app_config->state));

    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_1_ENABLED, &sensor_app_config->is_sensor_1_enabled, sizeof(sensor_app_config->is_sensor_1_enabled));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_1_POWER, &sensor_app_config->sensor_1_voltage, sizeof(sensor_app_config->sensor_1_voltage));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_1_TYPE, &sensor_app_config->sensor_1_type, sizeof(sensor_app_config->sensor_1_type));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_1_FREQUENCY, &sensor_app_config->sensor_1_frequency, sizeof(sensor_app_config->sensor_1_frequency));

    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_2_ENABLED, &sensor_app_config->is_sensor_2_enabled, sizeof(sensor_app_config->is_sensor_2_enabled));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_2_POWER, &sensor_app_config->sensor_2_voltage, sizeof(sensor_app_config->sensor_2_voltage));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_2_TYPE, &sensor_app_config->sensor_2_type, sizeof(sensor_app_config->sensor_2_type));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_2_FREQUENCY, &sensor_app_config->sensor_2_frequency, sizeof(sensor_app_config->sensor_2_frequency));
    return 0;
}

static int initialize_nvs(void)
{
    int ret;
    ret = sensor_nvs_setup(SENSOR_NVS_ADDRESS_LIMIT);
    if(ret < 0)
    {
        return ret;
    }
    /* Pull all data from NVS */
    ret = initialize_lorawan_nvs();
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize LoRaWAN NVS");
        return ret;
    }
    ret = initialize_sensor_nvs();
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize Sensor NVS");
        return ret;
    }
    return 0;
}

int sensor_app_init(sensor_app_config_t *config)
{
    int ret;
    sensor_app_config = config;
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