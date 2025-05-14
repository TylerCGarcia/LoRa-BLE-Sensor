
#include "sensor_data.h"
#include "sensor_power.h"
#include "sensor_reading.h"
#include <stdint.h>
#include <zephyr/kernel.h>
/* Library for memory allocation. */
#include <zephyr/sys/mem_manage.h>

// typedef struct data_type_info{
//     /* Type of data in the buffer. */
//     enum sensor_data_type data_type;
//     /* Size of the data in the buffer. */
//     size_t data_size;
// } data_type_info_t;

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor_data, LOG_LEVEL_DBG);

typedef struct {
    /* Power id to use for the sensor. */
    enum sensor_types type;
    /* Voltage enum to use for the sensor. */
    enum sensor_voltage voltage_enum;
    /* 1 if the power to sensor should always be on. 0 if only turned on when data is being read. */
    uint8_t is_sensor_power_continuous;
    // /* Type of data in the buffer. */
    // enum sensor_data_type data_type;
} sensor_data_config_t;

/* Sensor data configurations. */
static sensor_data_config_t sensor_data_config[SENSOR_INDEX_LIMIT];

/* Sensor power configurations for power 1. */
static sensor_power_config_t sensor_output1 = {
	.power_id = SENSOR_POWER_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output1),
	.delay_ms = 500
};

/* Sensor power configurations for power 2. */
static sensor_power_config_t sensor_output2 = {
	.power_id = SENSOR_POWER_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output2),
	.delay_ms = 500
};

/* Array of sensor power configurations. */
static sensor_power_config_t *sensor_power_configs[] = {
    &sensor_output1,
    &sensor_output2
};

sensor_reading_config_t sensor1_reading_config = {
    .id = SENSOR_1,
	.d1 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor1d1), gpios),	
	.d2 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor1d2), gpios),
	.voltage_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), voltage_sensor1),
    .current_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), current_sensor1)
};

sensor_reading_config_t sensor2_reading_config = {
    .id = SENSOR_2,
	.d1 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor2d1), gpios),	
	.d2 = GPIO_DT_SPEC_GET(DT_ALIAS(sensor2d2), gpios),
	.voltage_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), voltage_sensor2),
    .current_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), current_sensor2)
};

static sensor_reading_config_t *sensor_reading_configs[] = {
    &sensor1_reading_config,
    &sensor2_reading_config
};

int sensor_data_setup(sensor_data_t *sensor_data, enum sensor_types type, enum sensor_voltage voltage_enum)
{
    if (sensor_data->id >= SENSOR_INDEX_LIMIT || sensor_data->power_id >= SENSOR_POWER_INDEX_LIMIT
        || type >= SENSOR_TYPE_LIMIT || voltage_enum >= SENSOR_VOLTAGE_INDEX_LIMIT) {
        return -1;
    }
    sensor_power_init(&sensor_power_configs[sensor_data->power_id]);

    /* Set the sensor data config. */
    sensor_data_config[sensor_data->id].type = type;
    sensor_data_config[sensor_data->id].voltage_enum = voltage_enum;
    /* Setup the sensor reading configuration. */
    sensor_reading_setup(&sensor_reading_configs[sensor_data->id], type);

    if(sensor_data_config[sensor_data->id].type == PULSE_SENSOR) 
    {
        sensor_data_config[sensor_data->id].is_sensor_power_continuous = 1;
        /* If the sensor type is continuous, the power should be on all the time. */
        set_sensor_output(&sensor_power_configs[sensor_data->power_id], voltage_enum);
        // validate_output(&sensor_power_configs[sensor_data->power_id], voltage_enum, 10);
    }
    else
    {
        sensor_data_config[sensor_data->id].is_sensor_power_continuous = 0;
    }

    return 0;
}

int sensor_data_read(sensor_data_t *sensor_data, uint32_t timestamp)
{
    if(sensor_data_config[sensor_data->id].type == PULSE_SENSOR)
    {
        // Check if we need to allocate or reallocate the buffer
        if (sensor_data->data_buffer == NULL) {
            LOG_DBG("Allocating buffer for sensor %d", sensor_data->id);
            // Initial allocation for data_buffer, initialization is needed since buffer is void pointer
            sensor_data->data_buffer = k_malloc(sizeof(int) * sensor_data->max_samples);
            if (sensor_data->data_buffer == NULL) {
                return -1;  // Handle allocation failure
            }
            // Initial allocation
            sensor_data->timestamp_buffer = k_malloc(sizeof(uint32_t) * sensor_data->max_samples);
            if (sensor_data->timestamp_buffer == NULL) {
                return -1;  // Handle allocation failure
            }
        }
        // // Store the timestamp
        sensor_data->timestamp_buffer[sensor_data->num_samples] = timestamp;

        // Cast and store the value
        int *pulse_counts = (int *)sensor_data->data_buffer;
        pulse_counts[sensor_data->num_samples] = get_sensor_pulse_count(sensor_data);

        sensor_data->num_samples++;
        // Update the buffer size if needed
        sensor_data->buffer_size = sizeof(int) * (sensor_data->num_samples + 1);
    }
    else if(sensor_data_config[sensor_data->id].type == VOLTAGE_SENSOR)
    {
        // sensor_data->buffer = get_sensor_voltage_reading(sensor_data);
    }
    else if(sensor_data_config[sensor_data->id].type == CURRENT_SENSOR)
    {
        // sensor_data->buffer = get_sensor_current_reading(sensor_data);
    }

    return 0;
}

int sensor_data_clear(sensor_data_t *sensor_data)
{
    if(sensor_data->data_buffer != NULL)
    {
        k_free(sensor_data->data_buffer);
        sensor_data->data_buffer = NULL;  // Set to NULL after freeing
    }
    if(sensor_data->timestamp_buffer != NULL)
    {
        k_free(sensor_data->timestamp_buffer);
        sensor_data->timestamp_buffer = NULL;  // Set to NULL after freeing
    }
    sensor_data->num_samples = 0;
    sensor_data->buffer_size = 0;  // Also reset buffer size

    return 0;
}