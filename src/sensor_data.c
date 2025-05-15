
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
    /* 1 if the sensor is setup. 0 if the sensor is not setup. */
    uint8_t is_sensor_setup;
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

    uint32_t buffer_size = sensor_data->data_size * sensor_data->max_samples;
    uint32_t timestamp_buffer_size = sensor_data->timestamp_size * sensor_data->max_samples;

    sensor_data->data_ring_buf.buffer = k_malloc(buffer_size);
    sensor_data->timestamp_ring_buf.buffer = k_malloc(timestamp_buffer_size);

    ring_buf_init(&sensor_data->data_ring_buf, buffer_size, sensor_data->data_ring_buf.buffer);
    ring_buf_init(&sensor_data->timestamp_ring_buf, timestamp_buffer_size, sensor_data->timestamp_ring_buf.buffer);
    sensor_data_config[sensor_data->id].is_sensor_setup = 1;
    return 0;
}

int sensor_data_read(sensor_data_t *sensor_data, uint32_t timestamp)
{
    if (&sensor_data->data_ring_buf == NULL || &sensor_data->timestamp_ring_buf == NULL) {
        LOG_ERR("Ring buffers not initialized");
        return -1;  // Ring buffers not initialized
    }

    int ret;
    if(sensor_data_config[sensor_data->id].type == PULSE_SENSOR)
    {
        int pulse_count = get_sensor_pulse_count(sensor_reading_configs[sensor_data->id]);
        
        // // Put data into ring buffer
        ret = ring_buf_put(&sensor_data->data_ring_buf, (uint8_t *)&pulse_count, sensor_data->data_size);
        if (ret != sensor_data->data_size) {
            LOG_ERR("Failed to put data into data ring buffer");
            return -1;
        }
    }
    else if(sensor_data_config[sensor_data->id].type == VOLTAGE_SENSOR)
    {
        // sensor_data->buffer = get_sensor_voltage_reading(sensor_data);
    }
    else if(sensor_data_config[sensor_data->id].type == CURRENT_SENSOR)
    {
        // sensor_data->buffer = get_sensor_current_reading(sensor_data);
    }

    // Put timestamp into ring buffer
    ret = ring_buf_put(&sensor_data->timestamp_ring_buf, (uint8_t *)&timestamp, sensor_data->data_size);
    if (ret != sensor_data->timestamp_size) {
        LOG_ERR("Failed to put timestamp into timestamp ring buffer");
        return -1;
    }
    return 0;
}

int sensor_data_get_latest_reading(sensor_data_t *sensor_data, void *value, uint32_t *timestamp) 
{
    uint32_t size;
    int ret;
    // Get the latest timestamp
    size = ring_buf_get(&sensor_data->timestamp_ring_buf, (uint8_t *)timestamp, sensor_data->timestamp_size);
    if (size != sensor_data->timestamp_size) {
        LOG_ERR("Failed to get timestamp from timestamp ring buffer");
        return -1;
    }

    // Get the latest data
    if (sensor_data_config[sensor_data->id].type == PULSE_SENSOR) {
        // uint8_t data_bytes[sensor_data->data_size];
        size = ring_buf_get(&sensor_data->data_ring_buf, (uint8_t *)value,  sizeof(int));        
        if (size != sensor_data->data_size) {
            LOG_ERR("Failed to get data from data ring buffer");
            return -1;
        }
    }

    return 0;
}


int sensor_data_clear(sensor_data_t *sensor_data)
{
    /* If the sensor is setup, clear the ring buffers and reset the ring buffers. */
    if (sensor_data_config[sensor_data->id].is_sensor_setup == 1) 
    {
        if (sensor_data->data_ring_buf.buffer != NULL) 
        {
            k_free(sensor_data->data_ring_buf.buffer);
        }
        if (sensor_data->timestamp_ring_buf.buffer != NULL) {
            k_free(sensor_data->timestamp_ring_buf.buffer);
        }
        if (&sensor_data->data_ring_buf != NULL) 
        {
            ring_buf_reset(&sensor_data->data_ring_buf);  // Note: removed & since it's a pointer
        }
    
        if (&sensor_data->timestamp_ring_buf != NULL) 
        {
            ring_buf_reset(&sensor_data->timestamp_ring_buf);  // Note: removed & since it's a pointer
        }
        sensor_data_config[sensor_data->id].is_sensor_setup = 0;
    }
    return 0;
}