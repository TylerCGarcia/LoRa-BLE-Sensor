/**
 * @file sensor_app.c
 * @author Tyler Garcia
 * @brief A library to manage sensor applications. Working between
 * power configurations, data configurations, and scheduling for sensor reading. 
 * As well as LoRaWAN initialization, scheduling, and data transmission. This 
 * library also handles the BLE services for the device, sensor, and LoRaWAN.
 * @version 0.1
 * @date 2025-04-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "sensor_app.h"
#include "sensor_ble.h"
#include "sensor_scheduling.h"
#include "sensor_data.h"
#include "sensor_nvs.h"
#include "sensor_lorawan.h"
#include "sensor_pmic.h"
#include "ble_sensor_service.h"
#include "ble_lorawan_service.h"
#include "ble_device_service.h"
#include "sensor_names.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(SENSOR_APP, LOG_LEVEL_INF);

#define BLE_STACKSIZE			1024
#define BLE_THREAD_PRIORITY		1
/* Dynamic Threads */
K_THREAD_STACK_DEFINE(ble_stack, BLE_STACKSIZE);
struct k_thread blethread_id;

static sensor_app_config_t *sensor_app_config;

static sensor_scheduling_cfg_t sensor1_schedule = {
    .id = SENSOR_SCHEDULING_ID_SENSOR1,
    .frequency_seconds = 0
};

static sensor_scheduling_cfg_t sensor2_schedule = {
    .id = SENSOR_SCHEDULING_ID_SENSOR2,
    .frequency_seconds = 0
};

static sensor_scheduling_cfg_t radio_schedule = {
    .id = SENSOR_SCHEDULING_ID_RADIO,
    .frequency_seconds = 0
};

static lorawan_setup_t lorawan_setup = {
    .is_lorawan_enabled = 0,
    .lorawan_frequency = 0,
	.lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0)),
	.uplink_class = LORAWAN_CLASS_A,
	.downlink_callback = {0},
	.join_attempts = 20,
	.dev_nonce = 240,
	.delay = 1000,
	.dev_eui =  {0},
	.join_eui = {0},
	.app_key =  {0},
	.send_attempts = 10,
};
/* LoRaWAN data structure. */
static lorawan_data_t lorawan_data;

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

/* PMIC sensor status */
static pmic_sensor_status_t pmic_status;

/* The timer device to use for scheduling */
static const struct device *sensor_timer = DEVICE_DT_GET(DT_ALIAS(sensortimer));

static ble_config_t ble_config = {
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
        LOG_DBG("No data found in NVS address %d", address);
        if(sensor_nvs_write(address, data, size) < 0)
        {
            LOG_DBG("Failed to write data to NVS address %d", address);
            return ret;
        }
    }
    return 0;
}

static int initialize_lorawan_nvs(void)
{
    LOG_INF("Reading LoRaWAN NVS");
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_ENABLED, &lorawan_setup.is_lorawan_enabled, sizeof(lorawan_setup.is_lorawan_enabled));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_LORAWAN_FREQUENCY, &lorawan_setup.lorawan_frequency, sizeof(lorawan_setup.lorawan_frequency));
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
    get_sensor_voltage_name_from_index(sensor_app_config->sensor_1_voltage_name, sensor_app_config->sensor_1_voltage);
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_1_TYPE, &sensor_app_config->sensor_1_type, sizeof(sensor_app_config->sensor_1_type));
    get_sensor_type_name_from_index(sensor_app_config->sensor_1_type_name, sensor_app_config->sensor_1_type);
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_1_FREQUENCY, &sensor_app_config->sensor_1_frequency, sizeof(sensor_app_config->sensor_1_frequency));

    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_2_ENABLED, &sensor_app_config->is_sensor_2_enabled, sizeof(sensor_app_config->is_sensor_2_enabled));
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_2_POWER, &sensor_app_config->sensor_2_voltage, sizeof(sensor_app_config->sensor_2_voltage));
    get_sensor_voltage_name_from_index(sensor_app_config->sensor_2_voltage_name, sensor_app_config->sensor_2_voltage);
    initialize_nvs_address(SENSOR_NVS_ADDRESS_SENSOR_2_TYPE, &sensor_app_config->sensor_2_type, sizeof(sensor_app_config->sensor_2_type));
    get_sensor_type_name_from_index(sensor_app_config->sensor_2_type_name, sensor_app_config->sensor_2_type);
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

static int sensor_lorawan_connection(void)
{
    int ret;
    sensor_lorawan_log_network_config(&lorawan_setup);
    if(is_lorawan_configured(&lorawan_setup) == 0)
    {
        ret = sensor_lorawan_setup(&lorawan_setup);
        /* Save the dev nonce to NVS after connection attempt. */
        sensor_nvs_write(SENSOR_NVS_ADDRESS_LORAWAN_DEV_NONCE, &lorawan_setup.dev_nonce, sizeof(lorawan_setup.dev_nonce));
        if(ret < 0)
        {
            LOG_ERR("Failed to connect to LoRaWAN");
            return -1;
        }
        }
        else
        {
        LOG_INF("LoRaWAN is enabled but not configured");
    }
    return 0;
}

static int add_sensor_data_to_lorawan_payload(sensor_data_t *sensor_data)
{
    int ret;
    uint8_t sensor_data_buffer[(sensor_data->data_size * sensor_data->num_samples) + (sensor_data->timestamp_size * sensor_data->num_samples)];
    uint8_t sensor_data_buffer_len;
    ret = sensor_data_format_for_lorawan(sensor_data, sensor_data_buffer, &sensor_data_buffer_len);
    if(ret < 0)
    {
        LOG_ERR("Failed to format sensor data for LoRaWAN");
        return -1;
    }
    uint8_t initial_data_length = lorawan_data.length;
    for(int i = 0; i < sensor_data_buffer_len; i++)
    {
        lorawan_data.data[i + initial_data_length] = sensor_data_buffer[i];
    }
    lorawan_data.length = sensor_data_buffer_len + initial_data_length;

    return 0;
}

static int add_sensor_configuration_to_lorawan_payload(void)
{
    uint8_t i = lorawan_data.length;
    uint8_t hw_version_major;
    uint8_t hw_version_minor;
    uint8_t hw_version_patch; // Not added to payload yet
    sscanf(CONFIG_BOARD_REVISION, "%hhu.%hhu.%hhu", &hw_version_major, &hw_version_minor, &hw_version_patch);
    LOG_DBG("HW Version: %hhu.%hhu.%hhu", hw_version_major, hw_version_minor, hw_version_patch);
    lorawan_data.data[i++] = hw_version_major;
    lorawan_data.data[i++] = hw_version_minor;
    lorawan_data.data[i++] = CONFIG_APP_VERSION_MAJOR;
    lorawan_data.data[i++] = CONFIG_APP_VERSION_MINOR;
    lorawan_data.data[i++] = CONFIG_APP_VERSION_PATCH;
    lorawan_data.data[i++] = CONFIG_APP_VERSION_COMMIT;

    // Get timestamp and check for error
    int timestamp = sensor_scheduling_get_seconds();
    lorawan_data.data[i++] = (timestamp >> 24) & 0xFF;  // Most significant byte
    lorawan_data.data[i++] = (timestamp >> 16) & 0xFF;
    lorawan_data.data[i++] = (timestamp >> 8) & 0xFF;
    lorawan_data.data[i++] = timestamp & 0xFF;  

    // LoRaWAN configuration
    lorawan_data.data[i++] = lorawan_setup.lorawan_frequency;
    lorawan_data.data[i++] = lorawan_setup.send_attempts;
    // PMIC Information
    sensor_pmic_status_get(&pmic_status);
    // Break voltage into 2 bytes (high byte first, then low byte)
    int16_t voltage_hundreths = (int16_t)(pmic_status.voltage * 100.0f);
    lorawan_data.data[i++] = (voltage_hundreths >> 8) & 0xFF;  // High byte
    lorawan_data.data[i++] = voltage_hundreths & 0xFF;         // Low byte

    // Break temperature into 2 bytes (high byte first, then low byte)
    int16_t temperature_hundreths = (int16_t)(pmic_status.temp * 100.0f);
    lorawan_data.data[i++] = (temperature_hundreths >> 8) & 0xFF;  // High byte
    lorawan_data.data[i++] = temperature_hundreths & 0xFF;         // Low byte

    // Sensor configuration
    lorawan_data.data[i++] = (sensor_app_config->is_sensor_1_enabled << 0) | (sensor_app_config->is_sensor_2_enabled << 1);
    if(sensor_app_config->is_sensor_1_enabled)
    {
        LOG_DBG("Adding Sensor 1 configuration to LoRaWAN payload");
        lorawan_data.data[i++] = sensor_app_config->sensor_1_voltage;
        lorawan_data.data[i++] = sensor_app_config->sensor_1_type;
        lorawan_data.data[i++] = sensor_app_config->sensor_1_frequency;
    }
    if(sensor_app_config->is_sensor_2_enabled)
    {
        LOG_DBG("Adding Sensor 2 configuration to LoRaWAN payload");
        lorawan_data.data[i++] = sensor_app_config->sensor_2_voltage;
        lorawan_data.data[i++] = sensor_app_config->sensor_2_type;
        lorawan_data.data[i++] = sensor_app_config->sensor_2_frequency;
    }
    lorawan_data.length = i;
    LOG_DBG("Added %d bytes to payload for Sensor Configuration", i);
    return 0;
}

static int format_and_send_lorawan_payload(void)
{
    int ret;
    add_sensor_configuration_to_lorawan_payload();
    /* Add the sensor data to the LoRaWAN payload. */
    if(sensor_app_config->is_sensor_1_enabled)
    {
        add_sensor_data_to_lorawan_payload(&sensor1_data);
    }
    if(sensor_app_config->is_sensor_2_enabled)
    {
        add_sensor_data_to_lorawan_payload(&sensor2_data);
    }
    LOG_INF("Sending LoRaWAN payload with length %d", lorawan_data.length);
    lorawan_data.port = 2;
    lorawan_data.attempts = lorawan_setup.send_attempts;
    lorawan_data.delay = lorawan_setup.delay;
    ret = sensor_lorawan_send_data(&lorawan_data);
    memset(&lorawan_data, 0, sizeof(lorawan_data));
    if(ret < 0)
    {
        LOG_ERR("Failed to send LoRaWAN payload");
        return -1;
    }
    LOG_INF("LoRaWAN payload sent successfully");
    /* Clear the sensor data. */
    if(sensor_app_config->is_sensor_1_enabled)
    {
        sensor_data_clear(&sensor1_data);
    }
    if(sensor_app_config->is_sensor_2_enabled)
    {
        sensor_data_clear(&sensor2_data);
    }
    return 0;
}

/**
 * @brief Check that the app passes all the requirements to be in the running state.
 * 
 * @return int 0 on success, -1 on failure
 */
static int running_state_initialization_check(void)
{
    if(sensor_app_config->state != SENSOR_APP_STATE_RUNNING)
    {
        LOG_ERR("App is not in the running state");
        return -1;
    }
    /* Check that either sensor 1 or sensor 2 is enabled. */
    if(!sensor_app_config->is_sensor_1_enabled && !sensor_app_config->is_sensor_2_enabled)
    {
        LOG_ERR("Neither sensor 1 or sensor 2 is enabled");
        return -1;
    }
    /* Make sure that if sensor 1 is enabled, it is initialized to a valid sensor type and has a non-zero frequency. */
    if(sensor_app_config->is_sensor_1_enabled && sensor_app_config->sensor_1_type == NULL_SENSOR)
    {
        LOG_ERR("Sensor 1 is enabled but not initialized to any sensor types");
        sensor_app_config->is_sensor_1_enabled = 0;
        return -1;
    }
    if(sensor_app_config->is_sensor_1_enabled && sensor_app_config->sensor_1_frequency == 0)
    {
        LOG_ERR("Sensor 1 is enabled but has 0 frequency");
        sensor_app_config->is_sensor_1_enabled = 0;
        return -1;
    }
    /* Make sure that if sensor 2 is enabled, it is initialized to a valid sensor type and has a non-zero frequency. */
    if(sensor_app_config->is_sensor_2_enabled && sensor_app_config->sensor_2_frequency == 0)
    {
        LOG_ERR("Sensor 2 is enabled but has 0 frequency");
        sensor_app_config->is_sensor_2_enabled = 0;
        return -1;
    }
    if(sensor_app_config->is_sensor_2_enabled && sensor_app_config->sensor_2_type == NULL_SENSOR)
    {
        LOG_ERR("Sensor 2 is enabled but not initialized to any sensor types");
        sensor_app_config->is_sensor_2_enabled = 0;
        return -1;
    }
    /* Make sure that if lorawan is enabled, it has a valid frequency. */
    if(lorawan_setup.is_lorawan_enabled && lorawan_setup.lorawan_frequency == 0)
    {
        LOG_ERR("LoRaWAN is enabled but has 0 frequency");
        lorawan_setup.is_lorawan_enabled = 0;
        return -1;
    }
    return 0;
}

/**
 * @brief Initialize the sensor schedules.
 * 
 * @return int 0 on success, -1 on failure
 */
static int initialize_sensor_schedule(void)
{
    int ret;
    if(sensor_app_config->is_sensor_1_enabled && sensor_app_config->sensor_1_frequency > 0)
    {
        sensor1_schedule.frequency_seconds = MINUTES_TO_SECONDS(sensor_app_config->sensor_1_frequency);
        ret = sensor_scheduling_add_schedule(&sensor1_schedule);
        if(ret < 0)
        {
            LOG_ERR("Failed to add sensor 1 schedule");
        }
        else
        {
            sensor1_schedule.one_time_trigger = 1;
            LOG_INF("Sensor 1 schedule added at a frequency of %d seconds", sensor1_schedule.frequency_seconds);
        }
    }
    if(sensor_app_config->is_sensor_2_enabled && sensor_app_config->sensor_2_frequency > 0)
    {
        sensor2_schedule.frequency_seconds = MINUTES_TO_SECONDS(sensor_app_config->sensor_2_frequency);
        ret = sensor_scheduling_add_schedule(&sensor2_schedule);
        if(ret < 0)
        {
            LOG_ERR("Failed to add sensor 2 schedule");
        }
        else
        {
            sensor2_schedule.one_time_trigger = 1;
            LOG_INF("Sensor 2 schedule added at a frequency of %d seconds", sensor2_schedule.frequency_seconds);
        }
    }
    if(lorawan_setup.is_lorawan_enabled && lorawan_setup.lorawan_frequency > 0)
    {
        k_sleep(K_SECONDS(2)); // schedule radio to start after sensors are read
        radio_schedule.frequency_seconds = MINUTES_TO_SECONDS(lorawan_setup.lorawan_frequency);
        ret = sensor_scheduling_add_schedule(&radio_schedule);
        if(ret < 0)
        {
            LOG_ERR("Failed to add radio schedule");
        }
        else
        {
            radio_schedule.one_time_trigger = 1;
            LOG_INF("Radio schedule added at a frequency of %d seconds", radio_schedule.frequency_seconds);
        }
    }
    return 0;
}

/**
 * @brief Initialize the sensor data to read from the correct sensor configuration.
 * 
 * @return int 0 on success, -1 on failure
 */
static int initialize_sensor_data(void)
{
    int ret;
    if(sensor_app_config->is_sensor_1_enabled && sensor_app_config->sensor_1_type != NULL_SENSOR)
    {
        ret = sensor_data_setup(&sensor1_data, sensor_app_config->sensor_1_type, sensor_app_config->sensor_1_voltage);
        if(ret < 0)
        {
            LOG_ERR("Failed to initialize sensor 1 data");
            return ret;
        }
    }
    if(sensor_app_config->is_sensor_2_enabled && sensor_app_config->sensor_2_type != NULL_SENSOR)
    {
        ret = sensor_data_setup(&sensor2_data, sensor_app_config->sensor_2_type, sensor_app_config->sensor_2_voltage);
        if(ret < 0)
        {
            LOG_ERR("Failed to initialize sensor 2 data");
            return ret;
        }
    }
    return 0;
}

static int disable_sensor(void)
{
    int ret;
    if(sensor_app_config->is_sensor_1_enabled)
    {
        ret = sensor_data_setup(&sensor1_data, NULL_SENSOR, SENSOR_VOLTAGE_OFF);
        if(ret < 0)
        {
            LOG_ERR("Failed to disable sensor 1");
            return ret;
        }
        ret = sensor_scheduling_remove_schedule(&sensor1_schedule);
        if(ret < 0)
        {
            LOG_ERR("Failed to remove sensor 1 schedule");
            return ret;
        }
    }
    if(sensor_app_config->is_sensor_2_enabled)
    {
        ret = sensor_data_setup(&sensor2_data, NULL_SENSOR, SENSOR_VOLTAGE_OFF);
        if(ret < 0)
        {
            LOG_ERR("Failed to disable sensor 2");
            return ret;
        }
        ret = sensor_scheduling_remove_schedule(&sensor2_schedule);
        if(ret < 0)
        {
            LOG_ERR("Failed to remove sensor 2 schedule");
            return ret;
        }
    }
    LOG_INF("Sensors disabled");
    if(lorawan_setup.is_lorawan_enabled)
    {
        ret = sensor_scheduling_remove_schedule(&radio_schedule);
        if(ret < 0)
        {
            LOG_ERR("Failed to remove radio schedule");
            return ret;
        }
    }
    return 0;
}

static int update_sensor_data_timestamps(void)
{
    if(sensor_app_config->is_sensor_1_enabled)
    {
        sensor_app_config->sensor_1_latest_data_timestamp = (sensor_scheduling_get_seconds() - sensor1_data.latest_timestamp);
    }
    if(sensor_app_config->is_sensor_2_enabled)
    {
        sensor_app_config->sensor_2_latest_data_timestamp = (sensor_scheduling_get_seconds() - sensor2_data.latest_timestamp);
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
    ret = sensor_pmic_init();
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize PMIC");
        return ret;
    }
    return 0;
}

int sensor_app_configuration_state(void)
{
    int ret; 
    while(sensor_app_config->state == SENSOR_APP_STATE_CONFIGURATION)
    {
        LOG_DBG("App is in the configuration state");
        sensor_pmic_led_on();
        k_msleep(500);
        sensor_pmic_led_off();
        k_msleep(500);
        sensor_pmic_status_get(&pmic_status);
    }
    
    if(lorawan_setup.is_lorawan_enabled && sensor_app_config->connect_network_during_configuration)
    {
        sensor_pmic_led_on();
        ret = sensor_lorawan_connection();
        if(ret < 0)
        {
            LOG_ERR("Failed to connect to LoRaWAN, returning to configuration state");
            sensor_app_config->state = SENSOR_APP_STATE_CONFIGURATION;
            return ret;
        }
        sensor_pmic_led_off();
    }
    return 0;
}

int sensor_app_running_state(void)
{
    int ret;
    /* Check that the app passes all the requirements to be in the running state. */
    ret = running_state_initialization_check();
    if(ret < 0)
    {
        LOG_ERR("Failed to check running state initialization");
        sensor_app_config->state = SENSOR_APP_STATE_ERROR;
        return ret;
    }
    /* Connect to LoRaWAN if it was enabled and not connected to during configuration. */
    if(lorawan_setup.is_lorawan_enabled && !is_lorawan_connected())
    {
        sensor_pmic_led_on();
        ret = sensor_lorawan_connection();
        if(ret < 0)
        {
            LOG_ERR("Failed to connect to LoRaWAN");
            sensor_app_config->state = SENSOR_APP_STATE_ERROR;
            return ret;
        }
        sensor_pmic_led_off();
    }
    /* Initialize the sensor schedules. */
    ret = initialize_sensor_schedule();
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize sensor schedules");
        sensor_app_config->state = SENSOR_APP_STATE_ERROR;
        return ret;
    }
    /* Initialize the sensor data. */
    ret = initialize_sensor_data();
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize sensor data");
        sensor_app_config->state = SENSOR_APP_STATE_ERROR;
        return ret;
    }
    LOG_INF("LoRaWAN: %s", lorawan_setup.is_lorawan_enabled ? "Enabled" : "Disabled");
    LOG_INF("Sensor 1: INDEX: %d NAME: %s", sensor_app_config->sensor_1_type, sensor_app_config->sensor_1_type_name);
    LOG_INF("Sensor 1 Power: INDEX: %d NAME: %s", sensor_app_config->sensor_1_voltage, sensor_app_config->sensor_1_voltage_name);
    LOG_INF("Sensor 2: INDEX: %d NAME: %s", sensor_app_config->sensor_2_type, sensor_app_config->sensor_2_type_name);
    LOG_INF("Sensor 2 Power: INDEX: %d NAME: %s", sensor_app_config->sensor_2_voltage, sensor_app_config->sensor_2_voltage_name);

    while(sensor_app_config->state == SENSOR_APP_STATE_RUNNING)
    {
        if(sensor_app_config->is_sensor_1_enabled && (sensor1_schedule.is_triggered || sensor1_schedule.one_time_trigger))
		{
            sensor_pmic_led_on();
			sensor1_schedule.one_time_trigger = 0;
			LOG_INF("Sensor 1 schedule triggered");
			sensor_data_read(&sensor1_data, sensor_scheduling_get_seconds());
			sensor_data_print_data(&sensor1_data);
			sensor_scheduling_reset_schedule(&sensor1_schedule);
            sensor_app_config->sensor_1_latest_data = sensor1_data.latest_data;
            sensor_app_config->sensor_1_latest_data_timestamp = (sensor_scheduling_get_seconds() - sensor1_data.latest_timestamp);
            sensor_pmic_led_off();
        }
		if(sensor_app_config->is_sensor_2_enabled && (sensor2_schedule.is_triggered || sensor2_schedule.one_time_trigger))
		{
            sensor_pmic_led_on();
			sensor2_schedule.one_time_trigger = 0;
			LOG_INF("Sensor 2 schedule triggered");
			sensor_data_read(&sensor2_data, sensor_scheduling_get_seconds());
			sensor_data_print_data(&sensor2_data);
			sensor_scheduling_reset_schedule(&sensor2_schedule);
            sensor_app_config->sensor_2_latest_data = sensor2_data.latest_data;
            sensor_app_config->sensor_2_latest_data_timestamp = (sensor_scheduling_get_seconds() - sensor2_data.latest_timestamp);
            sensor_pmic_led_off();
        }
		if(lorawan_setup.is_lorawan_enabled && (radio_schedule.is_triggered || radio_schedule.one_time_trigger))
		{
            sensor_pmic_led_on();
			radio_schedule.one_time_trigger = 0;
			LOG_INF("Radio schedule triggered");

			sensor_scheduling_reset_schedule(&radio_schedule);
            /* Send the LoRaWAN payload. */
			ret = format_and_send_lorawan_payload();
            if(ret < 0)
            {
                LOG_ERR("Failed to send LoRaWAN payload");
            }
            sensor_pmic_led_off();
		}
        LOG_DBG("App is in the running state");
        k_msleep(1000);
        update_sensor_data_timestamps();
        sensor_pmic_status_get(&pmic_status);
    }
    /* Disable sensors.*/
    ret = disable_sensor();
    if(ret < 0)
    {
        LOG_ERR("Failed to disable sensors");
        sensor_app_config->state = SENSOR_APP_STATE_ERROR;
        return ret;
    }
    return 0;
}

int sensor_app_error_state(void)
{
    while(sensor_app_config->state == SENSOR_APP_STATE_ERROR)
    {
        LOG_ERR("App is in the error state");
        k_sleep(K_SECONDS(1));
    }
    return 0;
}

static void ble_thread(void *arg1, void *arg2, void *arg3)
{
    LOG_INF("BLE Thread Started");
    int ret;
    ret = ble_setup(&ble_config);
    LOG_INF("Setting up BLE Services");
    ret = ble_device_service_init(&pmic_status);
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize Device BLE service");
        sensor_app_config->state = SENSOR_APP_STATE_ERROR;
        return;
    }
    ret = ble_sensor_service_init(sensor_app_config);
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize BLE sensor service");
        sensor_app_config->state = SENSOR_APP_STATE_ERROR;
        return;
    }
    ret = ble_lorawan_service_init(&lorawan_setup);
    if(ret < 0)
    {
        LOG_ERR("Failed to initialize LoRaWAN BLE service");
        sensor_app_config->state = SENSOR_APP_STATE_ERROR;
        return;
    }
    ble_change_name(&ble_config);
    while(1)
    {
        LOG_DBG("BLE Thread Running");
        k_sleep(K_SECONDS(1));
    }
}

int sensor_app_ble_start(void)
{
    k_thread_create(&blethread_id, ble_stack, K_THREAD_STACK_SIZEOF(ble_stack), ble_thread, NULL, NULL, NULL, BLE_THREAD_PRIORITY, 0, K_NO_WAIT);
	k_thread_start(&blethread_id);
    return 0;
}