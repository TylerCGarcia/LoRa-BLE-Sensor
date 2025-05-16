/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - test correct power calls for each sensor type
 */

#include <zephyr/ztest.h>
#include "sensor_data.h"
#include "sensor_power_fakes.h"
#include "sensor_reading_fakes.h"
#include <zephyr/logging/log.h>
#include "sensor_id.h"
#include <zephyr/fff.h>
#include <zephyr/sys/ring_buffer.h>

LOG_MODULE_REGISTER(tests_data, LOG_LEVEL_INF);

DEFINE_FFF_GLOBALS;

sensor_data_t sensor1_data = {
    .id = SENSOR_1,
    .power_id = SENSOR_POWER_1,
    .max_samples = 10,
    .data_size = 4,
    .timestamp_size = 4,
};

sensor_data_t sensor2_data = {
    .id = SENSOR_2,
    .power_id = SENSOR_POWER_2,
    .max_samples = 10,
    .data_size = 4,
    .timestamp_size = 4,
};

/**
 * @brief Reset all fakes being used in tests
 * 
 */
static void reset_all_fakes(void)
{
    /* clear sensor power fakes */
	RESET_FAKE(sensor_power_init);
    RESET_FAKE(get_sensor_output);
    RESET_FAKE(set_sensor_output);
    RESET_FAKE(read_sensor_output);
    RESET_FAKE(validate_output);
    RESET_FAKE(get_sensor_voltage_name);
    /* clear sensor reading fakes */
    RESET_FAKE(sensor_reading_setup);
    RESET_FAKE(get_sensor_reading_setup);
    RESET_FAKE(get_sensor_voltage_reading);
    RESET_FAKE(get_sensor_current_reading);
    RESET_FAKE(get_sensor_pulse_count);
    RESET_FAKE(reset_sensor_pulse_count);
}

typedef struct {
    uint32_t initial_timestamp;
    uint32_t reading_interval;
    uint32_t num_samples;
    enum sensor_data_type data_type;
    void *fake_return_val;
} loop_data_t;


static void data_read_loop(sensor_data_t *sensor_data, loop_data_t *loop_data)
{
    int ret;
    for (uint32_t i = 0; i < loop_data->num_samples; i++) {
        uint32_t timestamp = loop_data->initial_timestamp + (i * loop_data->reading_interval);
        switch (loop_data->data_type) {
        case PULSE_SENSOR:
            get_sensor_pulse_count_fake.return_val = ((int *)loop_data->fake_return_val)[i];
            break;
        case VOLTAGE_SENSOR:
            get_sensor_voltage_reading_fake.return_val = ((float *)loop_data->fake_return_val)[i];
            break;
        case CURRENT_SENSOR:
            get_sensor_current_reading_fake.return_val = ((float *)loop_data->fake_return_val)[i];
            break;
        default:
            break;
        }
        ret = sensor_data_read(sensor_data, timestamp);
        zassert_ok(ret, "Sensor data read failed");
    }
}


/**
 * @brief Setup sensor power systems 
 * 
 */
static void *after_tests(void)
{
    reset_all_fakes();
    sensor1_data.id = SENSOR_1;
    sensor1_data.power_id = SENSOR_POWER_1;
    sensor2_data.id = SENSOR_2;
    sensor2_data.power_id = SENSOR_POWER_2;
    int ret = sensor_data_clear(&sensor1_data);
    zassert_ok(ret, "Sensor data clear failed");
    ret = sensor_data_clear(&sensor2_data);
    zassert_ok(ret, "Sensor data clear failed");
}

ZTEST_SUITE(data, NULL, NULL, NULL, after_tests, NULL);

/**
 * @brief Test that the sensor data can be setup
 * 
 */
ZTEST(data, test_sensor_data_setup)
{
    int ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    zassert_equal(sensor_power_init_fake.call_count, 1, "Sensor power init should be called");
    zassert_equal(sensor_reading_setup_fake.call_count, 1, "Sensor reading setup should be called");
    zassert_equal(sensor_reading_setup_fake.arg1_val, VOLTAGE_SENSOR, "Sensor reading setup should be called with VOLTAGE_SENSOR");
}

/**
 * @brief Test that the sensor data setup fails if the sensor id is out of bounds
 * 
 */
ZTEST(data, test_sensor_data_setup_sensor_id_out_of_bounds)
{
    sensor1_data.id = SENSOR_INDEX_LIMIT;
    int ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_not_ok(ret, "Sensor data setup sensor id out of bounds failed");
    zassert_equal(sensor_power_init_fake.call_count, 0, "Sensor power init should not be called for out of bounds sensor id");
}

/**
 * @brief Test that the sensor data setup fails if the sensor power id is out of bounds
 * 
 */
ZTEST(data, test_sensor_data_setup_sensor_power_id_out_of_bounds)
{
    sensor1_data.power_id = SENSOR_POWER_INDEX_LIMIT;
    int ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_not_ok(ret, "Sensor data setup sensor power id out of bounds failed");
    zassert_equal(sensor_power_init_fake.call_count, 0, "Sensor power init should not be called for out of bounds sensor power id");
}

/**
 * @brief Test that the sensor data setup fails if the sensor type is out of bounds
 * 
 */
ZTEST(data, test_sensor_data_setup_sensor_type_out_of_bounds)   
{
    int ret = sensor_data_setup(&sensor1_data, SENSOR_TYPE_LIMIT, SENSOR_VOLTAGE_3V3);
    zassert_not_ok(ret, "Sensor data setup sensor type out of bounds failed");
    zassert_equal(sensor_power_init_fake.call_count, 0, "Sensor power init should not be called for out of bounds sensor type");
}

/**
 * @brief Test that the sensor data setup fails if the sensor voltage is out of bounds
 * 
 */
ZTEST(data, test_sensor_data_setup_sensor_voltage_out_of_bounds)
{
    int ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_INDEX_LIMIT);
    zassert_not_ok(ret, "Sensor data setup sensor voltage out of bounds failed");
    zassert_equal(sensor_power_init_fake.call_count, 0, "Sensor power init should not be called for out of bounds sensor voltage");
}

/**
 * @brief Test that the sensor data setup sets the correct power and output for a PULSE_SENSOR
 * 
 */
ZTEST(data, test_sensor_data_setup_pulse_sensor)
{
    int ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    zassert_equal(sensor_power_init_fake.call_count, 1, "Sensor power init should be called");
    zassert_equal(set_sensor_output_fake.call_count, 1, "Set sensor output should be called");
    zassert_equal(sensor_reading_setup_fake.call_count, 1, "Sensor reading setup should be called");
    zassert_equal(sensor_reading_setup_fake.arg1_val, PULSE_SENSOR, "Sensor reading setup should be called with PULSE_SENSOR");
}

/**
 * @brief Test that the sensor can be read from the pulse sensor
 * 
 */
ZTEST(data, test_sensor_data_read_pulse_sensor)
{
    get_sensor_pulse_count_fake.return_val = 100;
    uint32_t timestamp = 1000;
    int ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
}

/**
 * @brief Test that the stores multiple samples correctly
 * 
 */
ZTEST(data, test_sensor_data_read_multiple_pulse_samples)
{
    int ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    loop_data_t loop_data = {
        .initial_timestamp = 1000,
        .num_samples = 15,
        .reading_interval = 100,
        .data_type = PULSE_SENSOR,
    };
    loop_data.fake_return_val = (void *)k_malloc(loop_data.num_samples * sizeof(int));
    for (uint32_t i = 0; i < loop_data.num_samples; i++) {
        ((int *)loop_data.fake_return_val)[i] = 100 + (i * 10);
    }
    data_read_loop(&sensor1_data, &loop_data);
    sensor_data_print_data(&sensor1_data);
    zassert_ok(ret, "Sensor data print failed");
    k_free(loop_data.fake_return_val);
}

/**
 * @brief Test that the sensor can be read from the voltage sensor
 * 
 */
ZTEST(data, test_sensor_data_read_voltage_sensor)
{
    get_sensor_voltage_reading_fake.return_val = 5.3;
    uint32_t timestamp = 1000;
    int ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
    sensor_data_print_data(&sensor1_data);
    zassert_ok(ret, "Sensor data print failed");
}

/**
 * @brief Test that the sensor can store multiple voltage samples
 * 
 */
ZTEST(data, test_sensor_data_read_multiple_voltage_samples)
{
    int ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_3V3);
    loop_data_t loop_data = {
        .initial_timestamp = 1000,
        .num_samples = 15,
        .reading_interval = 100,
        .data_type = VOLTAGE_SENSOR,
    };
    loop_data.fake_return_val = (void *)k_malloc(loop_data.num_samples * sizeof(int));
    for (uint32_t i = 0; i < loop_data.num_samples; i++) {
        ((float *)loop_data.fake_return_val)[i] = 1.0 + (i * 0.1);
    }
    zassert_ok(ret, "Sensor data setup failed");
    data_read_loop(&sensor1_data, &loop_data);
    sensor_data_print_data(&sensor1_data);
    zassert_ok(ret, "Sensor data print failed");
}

/**
 * @brief Test that the sensor can be read from the current sensor
 * 
 */
ZTEST(data, test_sensor_data_read_current_sensor)
{
    get_sensor_current_reading_fake.return_val = 0.5;
    uint32_t timestamp = 1000;
    int ret = sensor_data_setup(&sensor1_data, CURRENT_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
    zassert_equal(get_sensor_current_reading_fake.call_count, 1, "Sensor data get latest reading should be called");
}

/**
 * @brief Test that the sensor can store multiple voltage samples
 * 
 */
ZTEST(data, test_sensor_data_read_multiple_current_samples)
{
    int ret = sensor_data_setup(&sensor1_data, CURRENT_SENSOR, SENSOR_VOLTAGE_3V3);
    loop_data_t loop_data = {
        .initial_timestamp = 5000,
        .num_samples = 15,
        .reading_interval = 100,
        .data_type = CURRENT_SENSOR,
    };
    loop_data.fake_return_val = (void *)k_malloc(loop_data.num_samples * sizeof(int));
    for (uint32_t i = 0; i < loop_data.num_samples; i++) {
        ((float *)loop_data.fake_return_val)[i] = 5.0 + (i * 0.1);
    }
    zassert_ok(ret, "Sensor data setup failed");
    data_read_loop(&sensor1_data, &loop_data);
    sensor_data_print_data(&sensor1_data);
    zassert_ok(ret, "Sensor data print failed");
}

/**
 * @brief Test that sensor data clear
 * 
 */
ZTEST(data, test_sensor_data_clear)
{
    get_sensor_pulse_count_fake.return_val = 100;
    uint32_t timestamp = 1000;
    int ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
    zassert_equal(ring_buf_size_get(&sensor1_data.data_ring_buf), sensor1_data.data_size, "Data ring buffer should be empty");
    zassert_equal(ring_buf_size_get(&sensor1_data.timestamp_ring_buf), sensor1_data.timestamp_size, "Timestamp ring buffer should be empty");
    ret = sensor_data_clear(&sensor1_data);
    zassert_ok(ret, "Sensor data clear failed");
    zassert_equal(ring_buf_size_get(&sensor1_data.data_ring_buf), 0, "Data ring buffer should be empty");
    zassert_equal(ring_buf_size_get(&sensor1_data.timestamp_ring_buf), 0, "Timestamp ring buffer should be empty");
}

/**
 * @brief Test that the sensor data can be cleared and new readings can be stored
 * 
 */
ZTEST(data, test_sensor_data_clear_and_read_after_clear)
{
    get_sensor_pulse_count_fake.return_val = 100;
    uint32_t timestamp = 1000;
    int ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
    int value1 = *(int *)sensor1_data.latest_data;
    zassert_ok(ret, "Sensor data setup failed");
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
    zassert_equal(ring_buf_size_get(&sensor1_data.data_ring_buf), sensor1_data.data_size, "Data ring buffer should be empty");
    zassert_equal(ring_buf_size_get(&sensor1_data.timestamp_ring_buf), sensor1_data.timestamp_size, "Timestamp ring buffer should be empty");
    ret = sensor_data_clear(&sensor1_data);
    zassert_ok(ret, "Sensor data clear failed");
    zassert_equal(ring_buf_size_get(&sensor1_data.data_ring_buf), 0, "Data ring buffer should be empty");
    zassert_equal(ring_buf_size_get(&sensor1_data.timestamp_ring_buf), 0, "Timestamp ring buffer should be empty");
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
    zassert_equal(ring_buf_size_get(&sensor1_data.data_ring_buf), sensor1_data.data_size, "Data ring buffer should be empty");
    zassert_equal(ring_buf_size_get(&sensor1_data.timestamp_ring_buf), sensor1_data.timestamp_size, "Timestamp ring buffer should be empty");
    // Get and verify the reading
    int value = *(int *)sensor1_data.latest_data;
    uint32_t read_timestamp = sensor1_data.latest_timestamp;
    int expected_value = get_sensor_pulse_count_fake.return_val;
    zassert_ok(ret, "Failed to get latest reading");
    zassert_equal(value, expected_value, "Expected pulse count is %d, actual is %d", expected_value, value);
    ret = sensor_data_print_data(&sensor1_data);
    zassert_ok(ret, "Sensor data print failed");
}

/**
 * @brief Test that the correct power calls are made for a voltage sensor with 24V power
 * 
 */
ZTEST(data, test_sensor_data_power_calls_for_voltage_sensor_24v_power)
{
    int ret;
    uint32_t timestamp = 1000;
    ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_24V);
    zassert_ok(ret, "Sensor data setup failed");
    zassert_equal(sensor_power_init_fake.call_count, 1, "Sensor power init should be called");
    zassert_equal(set_sensor_output_fake.call_count, 1, "Set sensor output should be called once for VOLTAGE_SENSOR to turn off the power");
    zassert_equal(set_sensor_output_fake.arg1_history[0], SENSOR_VOLTAGE_OFF, "Set sensor output should be called with SENSOR_VOLTAGE_OFF");
    RESET_FAKE(set_sensor_output);
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
    zassert_equal(set_sensor_output_fake.call_count, 2, "Set sensor output should be called twice, once to enable and once to disable");
    zassert_equal(set_sensor_output_fake.arg1_history[0], SENSOR_VOLTAGE_24V, "Set sensor output should be called with SENSOR_VOLTAGE_3V3");
    zassert_equal(set_sensor_output_fake.arg1_history[1], SENSOR_VOLTAGE_OFF, "Set sensor output should be called with SENSOR_VOLTAGE_OFF");
}

/**
 * @brief Test that the correct power calls are made for a current sensor with 12V power
 * 
 */
ZTEST(data, test_sensor_data_power_calls_for_current_sensor_12v_power)
{
    int ret;
    uint32_t timestamp = 1000;
    ret = sensor_data_setup(&sensor1_data, CURRENT_SENSOR, SENSOR_VOLTAGE_12V);
    zassert_ok(ret, "Sensor data setup failed");
    zassert_equal(sensor_power_init_fake.call_count, 1, "Sensor power init should be called");
    zassert_equal(set_sensor_output_fake.call_count, 1, "Set sensor output should be called once for CURRENT_SENSOR to turn off the power");
    zassert_equal(set_sensor_output_fake.arg1_history[0], SENSOR_VOLTAGE_OFF, "Set sensor output should be called with SENSOR_VOLTAGE_OFF");
    RESET_FAKE(set_sensor_output);
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
    zassert_equal(set_sensor_output_fake.call_count, 2, "Set sensor output should be called twice, once to enable and once to disable");
    zassert_equal(set_sensor_output_fake.arg1_history[0], SENSOR_VOLTAGE_12V, "Set sensor output should be called with SENSOR_VOLTAGE_12V");
    zassert_equal(set_sensor_output_fake.arg1_history[1], SENSOR_VOLTAGE_OFF, "Set sensor output should be called with SENSOR_VOLTAGE_OFF");
}


/**
 * @brief Test that the correct power calls are made for a PULSE_SENSOR with 3.3V power
 * 
 */
ZTEST(data, test_sensor_data_power_calls_for_pulse_sensor_3v3_power)
{
    int ret;
    uint32_t timestamp = 1000;
    ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    zassert_equal(sensor_power_init_fake.call_count, 1, "Sensor power init should be called");
    zassert_equal(set_sensor_output_fake.call_count, 1, "Set sensor output should be called once for PULSE_SENSOR since it is continuous power");
    zassert_equal(set_sensor_output_fake.arg1_history[0], SENSOR_VOLTAGE_3V3, "Set sensor output should be called with SENSOR_VOLTAGE_3V3");
    RESET_FAKE(set_sensor_output);
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
    zassert_equal(set_sensor_output_fake.call_count, 0, "Set sensor output should not be called for PULSE_SENSOR during read since it is continuous power");
}

/**
 * @brief Test that the PULSE SENSOR power is turned off when the sensor is deinitialized with NULL sensor type
 * 
 */
ZTEST(data, test_sensor_data_deinit_sensor_with_null_sensor_type)
{
    int ret;
    uint32_t timestamp = 1000;
    ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    zassert_equal(sensor_power_init_fake.call_count, 1, "Sensor power init should be called");
    zassert_equal(set_sensor_output_fake.call_count, 1, "Set sensor output should be called once for PULSE_SENSOR since it is continuous power");
    zassert_equal(set_sensor_output_fake.arg1_history[0], SENSOR_VOLTAGE_3V3, "Set sensor output should be called with SENSOR_VOLTAGE_3V3");
    ret = sensor_data_setup(&sensor1_data, NULL_SENSOR, SENSOR_VOLTAGE_OFF);
    zassert_ok(ret, "Sensor data setup failed");
    zassert_equal(set_sensor_output_fake.call_count, 2, "Set sensor output should be called twice, once to enable and once to disable");
    zassert_equal(set_sensor_output_fake.arg1_history[1], SENSOR_VOLTAGE_OFF, "Set sensor output should be called with SENSOR_VOLTAGE_OFF");
}

/**
 * @brief Test that the sensor data can be formatted for LoRaWAN
 * 
 */
ZTEST(data, test_sensor_data_format_pulse_sensor_for_lorawan)
{
    int ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    loop_data_t loop_data = {
        .initial_timestamp = 1000,
        .num_samples = 10,
        .reading_interval = 100,
        .data_type = PULSE_SENSOR,
    };
    loop_data.fake_return_val = (void *)k_malloc(loop_data.num_samples * sizeof(int));
    for (uint32_t i = 0; i < loop_data.num_samples; i++) {
        ((int *)loop_data.fake_return_val)[i] = 100 + (i * 10);
    }
    data_read_loop(&sensor1_data, &loop_data);
    zassert_ok(ret, "Sensor data print failed");
    k_free(loop_data.fake_return_val);

    uint8_t data[(sensor1_data.data_size * sensor1_data.max_samples) + (sensor1_data.timestamp_size * sensor1_data.max_samples)];
    uint8_t data_len = 0;
    uint8_t expected_data_len = (ring_buf_size_get(&sensor1_data.data_ring_buf)) + (ring_buf_size_get(&sensor1_data.timestamp_ring_buf));
    ret = sensor_data_format_for_lorawan(&sensor1_data, data, &data_len);
    zassert_ok(ret, "Sensor data format for LoRaWAN failed");
    zassert_equal(data_len, expected_data_len, "Sensor data length was %d, expected %d", data_len, expected_data_len);
    // Check timestamp (first 4 bytes)
    uint32_t timestamp;
    memcpy(&timestamp, data, sizeof(uint32_t));
    zassert_equal(timestamp, 1000, "First timestamp should be 1000, got %u", timestamp);

    // Check data (next 4 bytes)
    int value;
    memcpy(&value, data + sizeof(uint32_t), sizeof(int));
    zassert_equal(value, 100, "First data value should be 100, got %d", value);
}