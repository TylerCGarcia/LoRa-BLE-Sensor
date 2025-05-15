/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - Confirm sensor data can be setup
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
    // .data_ring_buf = NULL,
    // .timestamp_ring_buf = NULL,
    // .data_buffer = NULL,
    // .timestamp_buffer = NULL,
    // .buffer_size = 0,
    // .data_size = sizeof(int),
    // .timestamp_size = sizeof(uint32_t),
    .data_size = 4,
    .timestamp_size = 4,
};

sensor_data_t sensor2_data = {
    .id = SENSOR_2,
    .power_id = SENSOR_POWER_2,
    .max_samples = 10,
    // .data_ring_buf = NULL,
    // .timestamp_ring_buf = NULL,
    // .data_buffer = NULL,
    // .timestamp_buffer = NULL,
    // .buffer_size = 0,
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
    zassert_equal(set_sensor_output_fake.call_count, 0, "Set sensor output should not be called for VOLTAGE_SENSOR");
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

// /**
//  * @brief Test that the sensor_get_latest_reading returns the newest reading for a PULSE_SENSOR
//  * 
//  */
// ZTEST(data, test_sensor_data_get_latest_reading_pulse_sensor)
// {
//     int ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
//     zassert_ok(ret, "Sensor data setup failed");
//     loop_data_t loop_data = {
//         .initial_timestamp = 1000,
//         .num_samples = 15,
//         .reading_interval = 100,
//         .data_type = PULSE_SENSOR,
//     };
//     loop_data.fake_return_val = (void *)k_malloc(loop_data.num_samples * sizeof(int));
//     for (uint32_t i = 0; i < loop_data.num_samples; i++) {
//         ((int *)loop_data.fake_return_val)[i] = 100 + (i * 10);
//     }
//     data_read_loop(&sensor1_data, &loop_data);
//     k_free(loop_data.fake_return_val);
//     // Get and verify the reading
//     int value;
//     uint32_t read_timestamp;
//     // Get latest reading
//     ret = sensor_data_get_latest_reading(&sensor1_data, &value, &read_timestamp);
//     int expected_value = ((int *)loop_data.fake_return_val)[loop_data.num_samples - 1];
//     zassert_ok(ret, "Failed to get latest reading");
//     zassert_equal(value, expected_value, "Expected pulse count is %d, actual is %d", expected_value, value);
// }

// /**
//  * @brief Test that the sensor_get_latest_reading returns the newest reading for a VOLTAGE_SENSOR
//  * 
//  */
// ZTEST(data, test_sensor_data_get_latest_reading_voltage_sensor)
// {
//     int ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_3V3);
//     zassert_ok(ret, "Sensor data setup failed");
//     loop_data_t loop_data = {
//         .initial_timestamp = 1000,
//         .num_samples = 15,
//         .reading_interval = 100,
//         .data_type = VOLTAGE_SENSOR,
//     };
//     loop_data.fake_return_val = (void *)k_malloc(loop_data.num_samples * sizeof(float));
//     for (uint32_t i = 0; i < loop_data.num_samples; i++) {
//         ((float *)loop_data.fake_return_val)[i] = 1.0 + (i * 0.1);
//     }
//     data_read_loop(&sensor1_data, &loop_data);
//     k_free(loop_data.fake_return_val);
//     // Get and verify the reading
//     float value;
//     uint32_t read_timestamp;
//     // Get latest reading
//     ret = sensor_data_get_latest_reading(&sensor1_data, &value, &read_timestamp);
//     float expected_value = ((float *)loop_data.fake_return_val)[loop_data.num_samples - 1];
//     zassert_ok(ret, "Failed to get latest reading");
//     zassert_equal(value, expected_value, "Expected voltage is %f, actual is %f", expected_value, value);
// }
        
// /**
//  * @brief Test that the sensor_get_latest_reading fails when there is no data
//  * 
//  */
// ZTEST(data, test_sensor_data_get_latest_reading_fails_with_no_data)
// {
//     int ret = sensor_data_setup(&sensor1_data, PULSE_SENSOR, SENSOR_VOLTAGE_3V3);
//     zassert_ok(ret, "Sensor data setup failed");
//     int value;
//     uint32_t read_timestamp;
//     ret = sensor_data_get_latest_reading(&sensor1_data, &value, &read_timestamp);
//     zassert_not_ok(ret, "Expected get latest reading to fail");
// }

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
