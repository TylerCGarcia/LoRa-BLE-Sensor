/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - Can be configured to analog or digital sensor
 * - can be configured to analog voltage or analog current 
 * - can be configured to digital sensor pulse 
 * - handles invalid sensor types 
 * - handles sensor timeouts
 */

#include <zephyr/ztest.h>

#include "sensor_data.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/adc/adc_emul.h>

sensor_data_config_t sensor1_data_config = {
    .id = SENSOR_1,
	.d1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.d2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.voltage_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), voltage_sensor1),
    .current_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), current_sensor1)
};

ZTEST_SUITE(data, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test that sensor can be set up for voltage read
 * 
 */
ZTEST(data, test_sensor_setup_voltage)
{
    int ret = sensor_data_setup(&sensor1_data_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");
}

/**
 * @brief Test that sensor can be set up for current read
 * 
 */
ZTEST(data, test_sensor_setup_current)
{
    int ret = sensor_data_setup(&sensor1_data_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");
    zassert_true(device_is_ready(sensor1_data_config.current_read.dev), "Sensor1 current read is not ready");
}

/**
 * @brief Test that sensor can be set up for pulse counting
 * 
 */
ZTEST(data, test_sesnor_setup_pulse)
{
    int ret = sensor_data_setup(&sensor1_data_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, PULSE_SENSOR, "setup did not equal CURRENT_SENSOR");
    zassert_true(device_is_ready(sensor1_data_config.d1.port), "Sensor1 current read is not ready"); 
}

/**
 * @brief Construct a new ZTEST object
 * 
 */
ZTEST(data, test_sensor_voltage_read)
{
    int ret = sensor_data_setup(&sensor1_data_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");
    float expected_output = 3.3;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_data_config.voltage_read.dev, sensor1_data_config.voltage_read.channel_id, emul_mv);
    float voltage = get_sensor_voltage_reading(&sensor1_data_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(voltage, expected_output, accepted_error, "Mismatch: got %f, expected %f", voltage, expected_output);
}