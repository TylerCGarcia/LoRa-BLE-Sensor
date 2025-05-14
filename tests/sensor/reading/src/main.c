/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - handles invalid sensor types 
 * - handles sensor timeouts
 * - Pulse sensor interrupt is disabled after sensor type is changed
 */

#include <zephyr/ztest.h>

#include "sensor_reading.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/adc/adc_emul.h>

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

static void emulate_pulses(sensor_reading_config_t *config, int num_pulses)
{
    for(int i = 0; i < num_pulses; i++)
    {
        k_msleep(PULSE_DEBOUNCE_MS);
        gpio_emul_input_set(config->d1.port, config->d1.pin, 1);
        gpio_emul_input_set(config->d1.port, config->d1.pin, 0);
    }
    gpio_emul_input_set(config->d1.port, config->d1.pin, 1);
}


/**
 * @brief Call after each test
 * 
 */
static void *after_tests(void)
{
	// Reset all sensors to NULL after each test
    sensor_reading_setup(&sensor1_reading_config, NULL_SENSOR);
    sensor_reading_setup(&sensor2_reading_config, NULL_SENSOR);
}


ZTEST_SUITE(reading, NULL, NULL, NULL, after_tests, NULL);

/**
 * @brief Test that sensor can be set up for voltage read
 * 
 */
ZTEST(reading, test_sensor_setup_voltage)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");
}

/**
 * @brief Test that sensor can be set up for current read
 * 
 */
ZTEST(reading, test_sensor_setup_current)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");
    zassert_true(device_is_ready(sensor1_reading_config.current_read.dev), "Sensor1 current read is not ready");
}

/**
 * @brief Test that sensor can be set up for pulse counting
 * 
 */
ZTEST(reading, test_sensor_setup_pulse)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, PULSE_SENSOR, "setup did not equal CURRENT_SENSOR");
    zassert_true(device_is_ready(sensor1_reading_config.d1.port), "Sensor1 current read is not ready"); 
}

/**
 * @brief Test that 2 sensor can be set up for 2 different settings
 * 
 */
ZTEST(reading, test_sesnor_setup_voltage_and_pulse)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");

    ret = sensor_reading_setup(&sensor2_reading_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup2 = get_sensor_reading_setup(&sensor2_reading_config);
    zassert_equal(setup2, PULSE_SENSOR, "setup did not equal CURRENT_SENSOR");
    zassert_true(device_is_ready(sensor2_reading_config.d1.port), "Sensor1 current read is not ready"); 
}

/**
 * @brief Test voltage read outputs expected value
 * 
 */
ZTEST(reading, test_sensor_voltage_read)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");
    float expected_output = 3.3;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_reading_config.voltage_read.dev, sensor1_reading_config.voltage_read.channel_id, emul_mv);
    float voltage = get_sensor_voltage_reading(&sensor1_reading_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(voltage, expected_output, accepted_error, "Mismatch: got %f, expected %f", voltage, expected_output);
}

/**
 * @brief Test voltage read works with both sensors
 * 
 */
ZTEST(reading, test_two_sensors_voltage_read)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");

    ret = sensor_reading_setup(&sensor2_reading_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup2 = get_sensor_reading_setup(&sensor2_reading_config);
    zassert_equal(setup2, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");

    float expected_output = 3.3;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_reading_config.voltage_read.dev, sensor1_reading_config.voltage_read.channel_id, emul_mv);

    float expected_output2 = 5;
    const uint16_t input_mv2 = (expected_output2 * 1000);
	const uint16_t emul_mv2 = (input_mv2 * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor2_reading_config.voltage_read.dev, sensor2_reading_config.voltage_read.channel_id, emul_mv2);
    
    float voltage = get_sensor_voltage_reading(&sensor1_reading_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(voltage, expected_output, accepted_error, "Mismatch: got %f, expected %f", voltage, expected_output);
    
    float voltage2 = get_sensor_voltage_reading(&sensor2_reading_config);
    float accepted_error2 = expected_output2 * 0.05; // Give 5% error
	zassert_within(voltage2, expected_output2, accepted_error2, "Mismatch: got %f, expected %f", voltage2, expected_output2);
}


/**
 * @brief Test voltage read zero expected value
 * 
 */
ZTEST(reading, test_sensor_voltage_read_zero)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");
    float expected_output = 0;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_reading_config.voltage_read.dev, sensor1_reading_config.voltage_read.channel_id, emul_mv);
    float voltage = get_sensor_voltage_reading(&sensor1_reading_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(voltage, expected_output, accepted_error, "Mismatch: got %f, expected %f", voltage, expected_output);
}

/**
 * @brief Test that the voltage read returns an error when sensor is not set up for voltage read
 * 
 */
ZTEST(reading, test_voltage_read_when_voltage_sensor_not_set)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    float expected_output = 0;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_reading_config.voltage_read.dev, sensor1_reading_config.voltage_read.channel_id, emul_mv);
    float voltage = get_sensor_voltage_reading(&sensor1_reading_config);
    zassert_not_ok(voltage, "Voltage sensor set when not expected");
}

/**
 * @brief Test current read outputs expected value
 * 
 */
ZTEST(reading, test_sensor_current_read)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed current setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");
    float expected_output = 7;
    const uint16_t input_ma = (expected_output);
	const uint16_t emul_mv = (input_ma) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor1_reading_config.current_read.dev, sensor1_reading_config.current_read.channel_id, emul_mv);
    float current = get_sensor_current_reading(&sensor1_reading_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(current, expected_output, accepted_error, "Mismatch: got %f, expected %f", current, expected_output);
}


/**
 * @brief Test current read outputs expected value of zero
 * 
 */
ZTEST(reading, test_sensor_current_read_zero)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed current setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");
    float expected_output = 0;
    const uint16_t input_ma = (expected_output);
	const uint16_t emul_mv = (input_ma) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor1_reading_config.current_read.dev, sensor1_reading_config.current_read.channel_id, emul_mv);
    float current = get_sensor_current_reading(&sensor1_reading_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(current, expected_output, accepted_error, "Mismatch: got %f, expected %f", current, expected_output);
}

/**
 * @brief Test current read outputs expected value of 24mA
 * 
 */
ZTEST(reading, test_sensor_current_read_24ma)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed current setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");
    float expected_output = 24;
    const uint16_t input_ma = (expected_output);
	const uint16_t emul_mv = (input_ma) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor1_reading_config.current_read.dev, sensor1_reading_config.current_read.channel_id, emul_mv);
    float current = get_sensor_current_reading(&sensor1_reading_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(current, expected_output, accepted_error, "Mismatch: got %f, expected %f", current, expected_output);
}

/**
 * @brief Test that the current read returns an error when sensor is not set up for current read
 * 
 */
ZTEST(reading, test_current_read_when_current_sensor_not_set)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    float expected_output = 24;
    const uint16_t input_ma = (expected_output);
	const uint16_t emul_mv = (input_ma) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor1_reading_config.current_read.dev, sensor1_reading_config.current_read.channel_id, emul_mv);
    float current = get_sensor_current_reading(&sensor1_reading_config);
    zassert_not_ok(current, "Voltage sensor set when not expected");
}

/**
 * @brief Test that the current read and voltage read can be used at the same time 
 * 
 */
ZTEST(reading, test_current_sensor_and_voltage_sensor)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, VOLTAGE_SENSOR);
    zassert_ok(ret, "Sensor1 failed voltage setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, VOLTAGE_SENSOR, "setup did not equal VOLTAGE_SENSOR");

    ret = sensor_reading_setup(&sensor2_reading_config, CURRENT_SENSOR);
    zassert_ok(ret, "Sensor1 failed current setup");
    enum sensor_types setup2 = get_sensor_reading_setup(&sensor2_reading_config);
    zassert_equal(setup2, CURRENT_SENSOR, "setup did not equal CURRENT_SENSOR");

    float expected_output = 3.3;
    const uint16_t input_mv = (expected_output * 1000);
	const uint16_t emul_mv = (input_mv * VOLTAGE_READ_DIVIDER_LOW) / (VOLTAGE_READ_DIVIDER_HIGH + VOLTAGE_READ_DIVIDER_LOW);
    adc_emul_const_value_set(sensor1_reading_config.voltage_read.dev, sensor1_reading_config.voltage_read.channel_id, emul_mv);

    float expected_current = 10;
	const uint16_t emul_mv2 = (expected_current) * CURRENT_READ_RESISTOR;
    adc_emul_const_value_set(sensor2_reading_config.current_read.dev, sensor2_reading_config.current_read.channel_id, emul_mv2);
 
    float voltage = get_sensor_voltage_reading(&sensor1_reading_config);
    float accepted_error = expected_output * 0.05; // Give 5% error
	zassert_within(voltage, expected_output, accepted_error, "Mismatch: got %f, expected %f", voltage, expected_output);
    
    float current = get_sensor_current_reading(&sensor2_reading_config);
    float accepted_error2 = expected_current * 0.05; // Give 5% error
	zassert_within(current, expected_current, accepted_error2, "Mismatch: got %f, expected %f", current, expected_output);
}

/**
 * @brief Test pulse read outputs expected value
 * 
 */
ZTEST(reading, test_sensor_pulse_read)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed pulse setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, PULSE_SENSOR, "setup did not equal PULSE_SENSOR");
    int expected_pulse_count = 10;
    emulate_pulses(&sensor1_reading_config, expected_pulse_count);
    int pulse_count = get_sensor_pulse_count(&sensor1_reading_config);
    zassert_equal(expected_pulse_count, pulse_count, "Expected %d, got %d", expected_pulse_count, pulse_count);
}

/**
 * @brief Test pulse read outputs expected value after a reset
 * 
 */
ZTEST(reading, test_sensor_pulse_reset)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed pulse setup");
    enum sensor_types setup = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup, PULSE_SENSOR, "setup did not equal PULSE_SENSOR");
    int expected_pulse_count = 0; // Should show 0 pulses after reset
    int fake_pulse_counts = 1; // Number of pulses to emulate
    emulate_pulses(&sensor1_reading_config, fake_pulse_counts);
    reset_sensor_pulse_count(&sensor1_reading_config);
    int pulse_count = get_sensor_pulse_count(&sensor1_reading_config);
    zassert_equal(expected_pulse_count, pulse_count, "Expected %d, got %d", expected_pulse_count, pulse_count);
}

/**
 * @brief Test pulse read outputs expected value from two sensors
 * 
 */
ZTEST(reading, test_sensor_pulse_read_two_sensors)
{
    int ret = sensor_reading_setup(&sensor1_reading_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor1 failed pulse setup");
    ret = sensor_reading_setup(&sensor2_reading_config, PULSE_SENSOR);
    zassert_ok(ret, "Sensor2 failed pulse setup");

    enum sensor_types setup1 = get_sensor_reading_setup(&sensor1_reading_config);
    zassert_equal(setup1, PULSE_SENSOR, "setup did not equal PULSE_SENSOR");

    enum sensor_types setup2 = get_sensor_reading_setup(&sensor2_reading_config);
    zassert_equal(setup2, PULSE_SENSOR, "setup did not equal PULSE_SENSOR");

    int expected_pulse_count1 = 5;
    emulate_pulses(&sensor1_reading_config, expected_pulse_count1);

    int expected_pulse_count2 = 14;
    emulate_pulses(&sensor2_reading_config, expected_pulse_count2);

    int pulse_count = get_sensor_pulse_count(&sensor1_reading_config);
    zassert_equal(expected_pulse_count1, pulse_count, "Sensor 1 expected %d, got %d", expected_pulse_count1, pulse_count);


    pulse_count = get_sensor_pulse_count(&sensor2_reading_config);
    zassert_equal(expected_pulse_count2, pulse_count, "Sensor 2 expected %d, got %d", expected_pulse_count2, pulse_count);
}

