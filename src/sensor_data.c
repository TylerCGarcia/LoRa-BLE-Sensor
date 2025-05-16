
#include "sensor_data.h"
#include "sensor_power.h"
#include "sensor_reading.h"
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor_data, LOG_LEVEL_INF);

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
        LOG_ERR("Sensor data setup failed, invalid sensor id, power id, type, or voltage enum");
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
        set_sensor_output(&sensor_power_configs[sensor_data->power_id], SENSOR_VOLTAGE_OFF);
    }

    // Free existing buffers if they exist
    if (sensor_data->data_ring_buf.buffer != NULL) {
        LOG_DBG("Freeing data ring buffer since it already exists");
        k_free(sensor_data->data_ring_buf.buffer);
    }
    if (sensor_data->timestamp_ring_buf.buffer != NULL) {
        LOG_DBG("Freeing timestamp ring buffer since it already exists");
        k_free(sensor_data->timestamp_ring_buf.buffer);
    }
    if (sensor_data->latest_data != NULL) {
        LOG_DBG("Freeing latest data since it already exists");
        k_free(sensor_data->latest_data);
    }

    // Allocate memory for latest data
    sensor_data->latest_data = k_malloc(sensor_data->data_size);
    if (sensor_data->latest_data == NULL) {
        LOG_ERR("Failed to allocate memory for latest data");
        return -1;
    }

    uint32_t buffer_size = sensor_data->data_size * sensor_data->max_samples;
    uint32_t timestamp_buffer_size = sensor_data->timestamp_size * sensor_data->max_samples;

    sensor_data->data_ring_buf.buffer = k_malloc(buffer_size);
    sensor_data->timestamp_ring_buf.buffer = k_malloc(timestamp_buffer_size);

    ring_buf_init(&sensor_data->data_ring_buf, buffer_size, sensor_data->data_ring_buf.buffer);
    ring_buf_init(&sensor_data->timestamp_ring_buf, timestamp_buffer_size, sensor_data->timestamp_ring_buf.buffer);
    sensor_data_config[sensor_data->id].is_sensor_setup = 1;
    sensor_data->num_samples = 0;
    return 0;
}

static int put_data_into_ring_buffer(sensor_data_t *sensor_data, void *data)
{
    int ret;
    uint32_t data_size = ring_buf_size_get(&sensor_data->data_ring_buf);
    if (data_size >= (sensor_data->data_size * sensor_data->max_samples))
    {
        uint8_t temp_data[sensor_data->data_size];
        LOG_DBG("Data ring buffer is full, removing oldest data");
        ring_buf_get(&sensor_data->data_ring_buf, (uint8_t *)temp_data, sensor_data->data_size);
    }
    else
    {
        sensor_data->num_samples++;
    }
    // Put data into ring buffer
    ret = ring_buf_put(&sensor_data->data_ring_buf, (uint8_t *)data, sensor_data->data_size);
    if (ret < 0) {
        LOG_ERR("Failed to put data into data ring buffer");
        return -1;
    }
    return 0;
}

static int put_timestamp_into_ring_buffer(sensor_data_t *sensor_data, int timestamp)
{
    int ret;
    uint32_t timestamp_size = ring_buf_size_get(&sensor_data->timestamp_ring_buf);
    if (timestamp_size >= (sensor_data->timestamp_size * sensor_data->max_samples))
    {
        uint8_t temp_timestamp[sensor_data->timestamp_size];
        LOG_DBG("Timestamp ring buffer is full, removing oldest data");
        ring_buf_get(&sensor_data->timestamp_ring_buf, (uint8_t *)temp_timestamp, sensor_data->timestamp_size);
    }
    // Put timestamp into ring buffer
    ret = ring_buf_put(&sensor_data->timestamp_ring_buf, (uint8_t *)&timestamp, sensor_data->timestamp_size);
    if (ret < 0) {
        LOG_ERR("Failed to put timestamp into timestamp ring buffer");
        return -1;
    }
    return 0;
}

int sensor_data_read(sensor_data_t *sensor_data, int timestamp)
{
    if (&sensor_data->data_ring_buf == NULL || &sensor_data->timestamp_ring_buf == NULL) {
        LOG_ERR("Ring buffers not initialized");
        return -1;  // Ring buffers not initialized
    }
    int ret;
    if (sensor_data_config[sensor_data->id].is_sensor_power_continuous == 0)
    {
        set_sensor_output(&sensor_power_configs[sensor_data->power_id], sensor_data_config[sensor_data->id].voltage_enum);
    }
    switch(sensor_data_config[sensor_data->id].type)
    {
        case PULSE_SENSOR:
        {
            int pulse_count = get_sensor_pulse_count(sensor_reading_configs[sensor_data->id]);
            memcpy(sensor_data->latest_data, &pulse_count, sensor_data->data_size);
            ret = put_data_into_ring_buffer(sensor_data, &pulse_count);
            if (ret < 0) {
                return -1;
            }
            break;
        }
        case VOLTAGE_SENSOR:
        {
            float voltage = get_sensor_voltage_reading(sensor_reading_configs[sensor_data->id]);
            memcpy(sensor_data->latest_data, &voltage, sensor_data->data_size);
            ret = put_data_into_ring_buffer(sensor_data, &voltage);
            if (ret < 0) {
                return -1;
            }
            break;
        }
        case CURRENT_SENSOR:
        {
            float current = get_sensor_current_reading(sensor_reading_configs[sensor_data->id]);
            memcpy(sensor_data->latest_data, &current, sensor_data->data_size);
            ret = put_data_into_ring_buffer(sensor_data, &current);
            if (ret < 0) {
                return -1;
            }
            break;
        }
    }
    sensor_data->latest_timestamp = timestamp;
    ret = put_timestamp_into_ring_buffer(sensor_data, timestamp);
    if (ret < 0) {
        return -1;
    }
    if (sensor_data_config[sensor_data->id].is_sensor_power_continuous == 0)
    {
        set_sensor_output(&sensor_power_configs[sensor_data->power_id], SENSOR_VOLTAGE_OFF);
    }
    return 0;
}

int sensor_data_print_data(sensor_data_t *sensor_data)
{
    uint32_t data_size = ring_buf_size_get(&sensor_data->data_ring_buf);
    uint32_t timestamp_size = ring_buf_size_get(&sensor_data->timestamp_ring_buf);
    
    LOG_INF("Ring Buffer Data size: %d bytes", data_size);
    LOG_INF("Ring Buffer Timestamp size: %d bytes", timestamp_size);
    
    // Calculate number of samples in the buffer
    size_t num_samples = data_size / sensor_data->data_size;
    LOG_INF("Number of samples in buffer: %d", num_samples);
    
    // Create temporary buffers
    uint8_t temp_data[sensor_data->data_size];
    uint8_t temp_timestamp[sensor_data->timestamp_size];
    
    // Read and print each sample
    for (size_t i = 0; i < num_samples; i++) {
        // Read data
        size_t size = ring_buf_get(&sensor_data->data_ring_buf, temp_data, sensor_data->data_size);
        if (size != sensor_data->data_size) {
            LOG_ERR("Failed to read data from ring buffer");
            return -1;
        }
        
        // Read timestamp
        size = ring_buf_get(&sensor_data->timestamp_ring_buf, temp_timestamp, sensor_data->timestamp_size);
        if (size != sensor_data->timestamp_size) {
            LOG_ERR("Failed to read timestamp from ring buffer");
            return -1;
        }
        uint32_t timestamp;
        memcpy(&timestamp, temp_timestamp, sensor_data->timestamp_size);
        // Print the data based on sensor type
        switch (sensor_data_config[sensor_data->id].type) {
            case PULSE_SENSOR:
                int pulse_count;
                memcpy(&pulse_count, temp_data, sensor_data->data_size);
                LOG_INF("Sample %d: Pulse Count = %d, Timestamp = %u", i, pulse_count, timestamp);
                break;
            case VOLTAGE_SENSOR:
                float voltage;
                memcpy(&voltage, temp_data, sensor_data->data_size);
                LOG_INF("Sample %d: Voltage = %f, Timestamp = %u", i, voltage, timestamp);
                break;
            case CURRENT_SENSOR:
                float current;
                memcpy(&current, temp_data, sensor_data->data_size);
                LOG_INF("Sample %d: Current = %f, Timestamp = %u", i, current, timestamp);
                break;
            default:
                break;
        }
        
        // Put the data back
        size = ring_buf_put(&sensor_data->data_ring_buf, temp_data, sensor_data->data_size);
        if (size != sensor_data->data_size) {
            LOG_ERR("Failed to put data back into ring buffer");
            return -1;
        }
        
        // Put the timestamp back
        size = ring_buf_put(&sensor_data->timestamp_ring_buf, temp_timestamp, sensor_data->timestamp_size);
        if (size != sensor_data->timestamp_size) {
            LOG_ERR("Failed to put timestamp back into ring buffer");
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
        if (&sensor_data->data_ring_buf != NULL) 
        {
            LOG_DBG("Resetting data ring buffer");
            ring_buf_reset(&sensor_data->data_ring_buf);  // Note: removed & since it's a pointer
        }
    
        if (&sensor_data->timestamp_ring_buf != NULL) 
        {
            LOG_DBG("Resetting timestamp ring buffer");
            ring_buf_reset(&sensor_data->timestamp_ring_buf);  // Note: removed & since it's a pointer
        }
        sensor_data->num_samples = 0;
    }
    return 0;
}

int sensor_data_format_for_lorawan(sensor_data_t *sensor_data, uint8_t *data, uint8_t *data_len)
{
    LOG_DBG("Formatting sensor data for LoRaWAN Transmission");
    // Get the size of data in the ring buffer
    uint32_t data_size = ring_buf_size_get(&sensor_data->data_ring_buf);
    uint32_t timestamp_size = ring_buf_size_get(&sensor_data->timestamp_ring_buf);
    
    // Calculate number of samples
    size_t num_samples = data_size / sensor_data->data_size;
    if (num_samples == 0) {
        LOG_ERR("No data in buffer");
        return -1;
    }

    // Create temporary buffers for the entire data
    uint8_t temp_data[data_size];
    uint8_t temp_timestamp[timestamp_size];
    
    // Peek all data at once
    size_t size = ring_buf_peek(&sensor_data->data_ring_buf, temp_data, data_size);
    if (size != data_size) {
        LOG_ERR("Failed to peek data from ring buffer");
        return -1;
    }
    
    // Peek all timestamps at once
    size = ring_buf_peek(&sensor_data->timestamp_ring_buf, temp_timestamp, timestamp_size);
    if (size != timestamp_size) {
        LOG_ERR("Failed to peek timestamps from ring buffer");
        return -1;
    }

    // Calculate total size needed for the formatted data
    *data_len = num_samples * (sensor_data->timestamp_size + sensor_data->data_size);
    
    // Format the data
    for (size_t i = 0; i < num_samples; i++) {
        // Calculate offsets
        size_t data_offset = i * sensor_data->data_size;
        size_t timestamp_offset = i * sensor_data->timestamp_size;
        size_t output_offset = i * (sensor_data->timestamp_size + sensor_data->data_size);
        
        // Copy timestamp first (4 bytes)
        memcpy(data + output_offset, 
               temp_timestamp + timestamp_offset, 
               sensor_data->timestamp_size);
        
        // Copy data next (4 bytes)
        memcpy(data + output_offset + sensor_data->timestamp_size, 
               temp_data + data_offset, 
               sensor_data->data_size);
    }
    
    return 0;
}