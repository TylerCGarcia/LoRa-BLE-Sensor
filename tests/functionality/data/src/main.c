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
#include <zephyr/logging/log.h>
#include "sensor_id.h"
#include <zephyr/fff.h>

LOG_MODULE_REGISTER(tests_data, LOG_LEVEL_INF);

DEFINE_FFF_GLOBALS;

sensor_data_t sensor1_data = {
    .id = SENSOR_1,
    .power_id = SENSOR_POWER_1,
};

sensor_data_t sensor2_data = {
    .id = SENSOR_2,
    .power_id = SENSOR_POWER_2,
};

/**
 * @brief Reset all fakes being used in tests
 * 
 */
static void reset_all_fakes(void)
{
	// Clear previous call history
	RESET_FAKE(sensor_power_init);
    RESET_FAKE(get_sensor_output);
    RESET_FAKE(set_sensor_output);
    RESET_FAKE(read_sensor_output);
    RESET_FAKE(validate_output);
    RESET_FAKE(get_sensor_voltage_name);
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
}

/**
 * @brief Test that the sensor can be read from
 * 
 */
ZTEST(data, test_sensor_data_read)
{
    uint32_t timestamp = 1000;
    int ret = sensor_data_setup(&sensor1_data, VOLTAGE_SENSOR, SENSOR_VOLTAGE_3V3);
    zassert_ok(ret, "Sensor data setup failed");
    ret = sensor_data_read(&sensor1_data, timestamp);
    zassert_ok(ret, "Sensor data read failed");
}





