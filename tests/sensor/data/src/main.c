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

sensor_data_config_t sensor2_data_config = {
    .id = SENSOR_2,
	.d1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.d2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.voltage_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), voltage_sensor2),
    .current_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), current_sensor2)
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
ZTEST(data, test_sensor_setup_pulse)
{
    int ret = sensor_data_setup(&sensor1_data_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, PULSE_SENSOR, "setup did not equal CURRENT_SENSOR");
    zassert_true(device_is_ready(sensor1_data_config.d1.port), "Sensor1 current read is not ready"); 
}

/**
 * @brief Test that 2 sensor can be set up for 2 different settings
 * 
 */
ZTEST(data, test_sesnor_setup_voltage_and_pulse)
{
    int ret = sensor_data_setup(&sensor1_data_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");

    ret = sensor_data_setup(&sensor2_data_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup2 = get_sensor_data_setup(&sensor2_data_config);
    zassert_equal(setup2, PULSE_SENSOR, "setup did not equal CURRENT_SENSOR");
    zassert_true(device_is_ready(sensor2_data_config.d1.port), "Sensor1 current read is not ready"); 
}

/**
 * @brief Test voltage read outputs expected value
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

/**
 * @brief Test voltage read works with both sensors
 * 
 */
ZTEST(data, test_two_sensors_voltage_read)
{
    int ret = sensor_data_setup(&sensor1_data_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");

    ret = sensor_data_setup(&sensor2_data_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup2 = get_sensor_data_setup(&sensor2_data_config);
    zassert_equal(setup2, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");

    float expected_output = 3.3;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_data_config.voltage_read.dev, sensor1_data_config.voltage_read.channel_id, emul_mv);

    float expected_output2 = 5;
    const uint16_t input_mv2 = (expected_output2 * 1000);
	const uint16_t emul_mv2 = (input_mv2 * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor2_data_config.voltage_read.dev, sensor2_data_config.voltage_read.channel_id, emul_mv2);
    
    float voltage = get_sensor_voltage_reading(&sensor1_data_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(voltage, expected_output, accepted_error, "Mismatch: got %f, expected %f", voltage, expected_output);
    
    float voltage2 = get_sensor_voltage_reading(&sensor2_data_config);
    float accepted_error2 = expected_output2 * 0.05; // Give 5% error
	zassert_within(voltage2, expected_output2, accepted_error2, "Mismatch: got %f, expected %f", voltage2, expected_output2);
}


/**
 * @brief Test voltage read zero expected value
 * 
 */
ZTEST(data, test_sensor_voltage_read_zero)
{
    int ret = sensor_data_setup(&sensor1_data_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");
    float expected_output = 0;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_data_config.voltage_read.dev, sensor1_data_config.voltage_read.channel_id, emul_mv);
    float voltage = get_sensor_voltage_reading(&sensor1_data_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(voltage, expected_output, accepted_error, "Mismatch: got %f, expected %f", voltage, expected_output);
}

/**
 * @brief Test that the voltage read returns an error when sensor is not set up for voltage read
 * 
 */
ZTEST(data, test_voltage_read_when_voltage_sensor_not_set)
{
    int ret = sensor_data_setup(&sensor1_data_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    float expected_output = 0;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_data_config.voltage_read.dev, sensor1_data_config.voltage_read.channel_id, emul_mv);
    float voltage = get_sensor_voltage_reading(&sensor1_data_config);
    zassert_not_ok(voltage, "Voltage sensor set when not expected");
}

/**
 * @brief Test current read outputs expected value
 * 
 */
ZTEST(data, test_sensor_current_read)
{
    int ret = sensor_data_setup(&sensor1_data_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed current setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");
    float expected_output = 7;
    const uint16_t input_ma = (expected_output);
	const uint16_t emul_mv = (input_ma) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor1_data_config.current_read.dev, sensor1_data_config.current_read.channel_id, emul_mv);
    float current = get_sensor_current_reading(&sensor1_data_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(current, expected_output, accepted_error, "Mismatch: got %f, expected %f", current, expected_output);
}


/**
 * @brief Test current read outputs expected value of zero
 * 
 */
ZTEST(data, test_sensor_current_read_zero)
{
    int ret = sensor_data_setup(&sensor1_data_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed current setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");
    float expected_output = 0;
    const uint16_t input_ma = (expected_output);
	const uint16_t emul_mv = (input_ma) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor1_data_config.current_read.dev, sensor1_data_config.current_read.channel_id, emul_mv);
    float current = get_sensor_current_reading(&sensor1_data_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(current, expected_output, accepted_error, "Mismatch: got %f, expected %f", current, expected_output);
}

/**
 * @brief Test current read outputs expected value of 24mA
 * 
 */
ZTEST(data, test_sensor_current_read_24ma)
{
    int ret = sensor_data_setup(&sensor1_data_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed current setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");
    float expected_output = 24;
    const uint16_t input_ma = (expected_output);
	const uint16_t emul_mv = (input_ma) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor1_data_config.current_read.dev, sensor1_data_config.current_read.channel_id, emul_mv);
    float current = get_sensor_current_reading(&sensor1_data_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(current, expected_output, accepted_error, "Mismatch: got %f, expected %f", current, expected_output);
}

/**
 * @brief Test that the current read returns an error when sensor is not set up for current read
 * 
 */
ZTEST(data, test_current_read_when_current_sensor_not_set)
{
    int ret = sensor_data_setup(&sensor1_data_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    float expected_output = 24;
    const uint16_t input_ma = (expected_output);
	const uint16_t emul_mv = (input_ma) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor1_data_config.current_read.dev, sensor1_data_config.current_read.channel_id, emul_mv);
    float current = get_sensor_current_reading(&sensor1_data_config);
    zassert_not_ok(current, "Voltage sensor set when not expected");
}

/**
 * @brief Test that the current read and voltage read can be used at the same time 
 * 
 */
ZTEST(data, test_current_sensor_and_voltage_sensor)
{
    int ret = sensor_data_setup(&sensor1_data_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");

    ret = sensor_data_setup(&sensor2_data_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed current setup");
    enum sensor_types setup2 = get_sensor_data_setup(&sensor2_data_config);
    zassert_equal(setup2, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");

    float expected_output = 3.3;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_data_config.voltage_read.dev, sensor1_data_config.voltage_read.channel_id, emul_mv);

    float expected_current = 10;
	const uint16_t emul_mv2 = (expected_current) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor2_data_config.current_read.dev, sensor2_data_config.current_read.channel_id, emul_mv2);
 
    float voltage = get_sensor_voltage_reading(&sensor1_data_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(voltage, expected_output, accepted_error, "Mismatch: got %f, expected %f", voltage, expected_output);
    
    float current = get_sensor_current_reading(&sensor2_data_config);
    float accepted_error2 = expected_current * 0.05; // Give 5% error
	zassert_within(current, expected_current, accepted_error2, "Mismatch: got %f, expected %f", current, expected_output);
}

/**
 * @brief Test pulse read outputs expected value
 * 
 */
ZTEST(data, test_sensor_pulse_read)
{
    int ret = sensor_data_setup(&sensor1_data_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed pulse setup");
    enum sensor_types setup = get_sensor_data_setup(&sensor1_data_config);
    zassert_equal(setup, PULSE_SENSOR, "setup did not equal PULSE_SENSOR");
    int expected_pulse_count = 1;
    int pulse_count = get_sensor_pulse_count(&sensor1_data_config);
    zassert_equal(expected_pulse_count, pulse_count, "Expected %d, got %d", expected_pulse_count, pulse_count);
}