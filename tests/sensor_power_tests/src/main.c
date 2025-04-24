/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - Regulators are less than 3V when off
 * - Regulator outputs are validated with ADC
 */

#include <zephyr/fff.h>
#include <zephyr/ztest.h>
#include "sensor_power.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/regulator/fake.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/adc/adc_emul.h>


DEFINE_FFF_GLOBALS;

// static const struct adc_dt_spec adc_bat = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), v_bat);

sensor_power_config_t sensor_output1 = {
	.output_id = SENSOR_OUTPUT_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output1)
};

sensor_power_config_t sensor_output2 = {
	.output_id = SENSOR_OUTPUT_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output2)
};

/**
 * @brief confirm_voltage checks that gpios are set correctly for the boost converter and the correct 
 * regulator calls are made.
 * 
 * @param config the sensor struct to check
 * @param voltage the voltage setting being confirmed
 */
static void confirm_voltage(sensor_power_config_t *config, enum sensor_voltage voltage)
{
	int val_en = gpio_emul_output_get(config->boost_en.port, config->boost_en.pin);
	int val_ctrl1 = gpio_emul_output_get(config->boost_ctrl1.port, config->boost_ctrl1.pin);
	int val_ctrl2 = gpio_emul_output_get(config->boost_ctrl2.port, config->boost_ctrl2.pin);

	int sensor_voltage_value = get_sensor_output(config);

	switch (voltage) {
		case SENSOR_VOLTAGE_OFF:
			zassert_equal(sensor_voltage_value, SENSOR_VOLTAGE_OFF, "Sensor output should be set to SENSOR_VOLTAGE_OFF instead is %d", sensor_voltage_value);
			zassert_false(regulator_is_enabled(config->ldo_dev), "Regulator should be off for SENSOR_VOLTAGE_OFF");
			zassert_equal(val_en, 0, "BOOST_EN should be LOW for OFF");
			zassert_equal(val_ctrl1, 0, "CTRL1 should be LOW for OFF");
			zassert_equal(val_ctrl2, 0, "CTRL2 should be LOW for OFF");
			break;
		case SENSOR_VOLTAGE_3V3:
			zassert_equal(sensor_voltage_value, SENSOR_VOLTAGE_3V3, "Sensor output should be set to SENSOR_VOLTAGE_3V3");
			zassert_true(regulator_is_enabled(config->ldo_dev), "Regulator should be on for SENSOR_VOLTAGE_3V3");
			zassert_equal(val_en, 0, "BOOST_EN should be LOW for SENSOR_VOLTAGE_3V3");
			zassert_equal(val_ctrl1, 0, "CTRL1 should be LOW for SENSOR_VOLTAGE_3V3");
			zassert_equal(val_ctrl2, 0, "CTRL2 should be LOW for SENSOR_VOLTAGE_3V3");
			break;
		case SENSOR_VOLTAGE_5V:
			zassert_equal(regulator_fake_enable_fake.call_count, 0, "Expected regulator_enable to not be called for for SENSOR_VOLTAGE_5V");
			zassert_false(regulator_is_enabled(config->ldo_dev), "Regulator should be off for SENSOR_VOLTAGE_5V");
			zassert_equal(val_en, 1, "BOOST_EN should be HIGH for SENSOR_VOLTAGE_5V");
			zassert_equal(val_ctrl1, 1, "CTRL1 should be HIGH for SENSOR_VOLTAGE_5V");
			zassert_equal(val_ctrl2, 1, "CTRL2 should be HIGH for SENSOR_VOLTAGE_5V");
			break;
		case SENSOR_VOLTAGE_6V:
			zassert_equal(regulator_fake_enable_fake.call_count, 0, "Expected regulator_enable to not be called for for SENSOR_VOLTAGE_6V");
			zassert_false(regulator_is_enabled(config->ldo_dev), "Regulator should be off for SENSOR_VOLTAGE_6V");
			zassert_equal(val_en, 1, "BOOST_EN should be HIGH for SENSOR_VOLTAGE_6V");
			zassert_equal(val_ctrl1, 0, "CTRL1 should be LOW for SENSOR_VOLTAGE_6V");
			zassert_equal(val_ctrl2, 1, "CTRL2 should be HIGH for SENSOR_VOLTAGE_6V");
			break;
		case SENSOR_VOLTAGE_12V:
			zassert_equal(regulator_fake_enable_fake.call_count, 0, "Expected regulator_enable to not be called for for SENSOR_VOLTAGE_12V");
			zassert_false(regulator_is_enabled(config->ldo_dev), "Regulator should be off for SENSOR_VOLTAGE_12V");
			zassert_equal(val_en, 1, "BOOST_EN should be HIGH for SENSOR_VOLTAGE_12V");
			zassert_equal(val_ctrl1, 1, "CTRL1 should be HIGH for SENSOR_VOLTAGE_12V");
			zassert_equal(val_ctrl2, 0, "CTRL2 should be LOW for SENSOR_VOLTAGE_12V");
			break;
		case SENSOR_VOLTAGE_24V:
			zassert_equal(regulator_fake_enable_fake.call_count, 0, "Expected regulator_enable to not be called for for SENSOR_VOLTAGE_24V");
			zassert_false(regulator_is_enabled(config->ldo_dev), "Regulator should be off for SENSOR_VOLTAGE_24V");
			zassert_equal(val_en, 1, "BOOST_EN should be HIGH for SENSOR_VOLTAGE_24V");
			zassert_equal(val_ctrl1, 0, "CTRL1 should be LOW for SENSOR_VOLTAGE_24V");
			zassert_equal(val_ctrl2, 0, "CTRL2 should be LOW for SENSOR_VOLTAGE_24V");
			break;
		default:
			ztest_test_fail();
			return -EINVAL; 
	}
}

/**
 * @brief Reset all fakes being used in tests
 * 
 */
static void reset_all_fakes(void)
{
	// Clear previous call history
	RESET_FAKE(regulator_fake_set_voltage);
	RESET_FAKE(regulator_fake_enable);
	RESET_FAKE(regulator_fake_disable);
}

/**
 * @brief Setup sensor power systems 
 * 
 */
static void testsuite_setup(void)
{
	reset_all_fakes();

	int err;
	err = sensor_power_init(&sensor_output1);
	zassert_ok(err, "Sensor power initialization failed");

	err = sensor_power_init(&sensor_output2);
	zassert_ok(err, "Sensor power initialization failed");

	zassert_true(device_is_ready(sensor_output1.output_read.dev), "Sensor1 ADC is not ready");
	zassert_true(device_is_ready(sensor_output2.output_read.dev), "Sensor2 ADC is not ready");

	zassert_true(device_is_ready(sensor_output1.ldo_dev), "LDO1 not ready");
	zassert_true(device_is_ready(sensor_output2.ldo_dev), "LDO2 not ready");

	zassert_true(device_is_ready(sensor_output1.boost_en.port), "GPIO EN1 not ready");
	zassert_true(device_is_ready(sensor_output1.boost_ctrl1.port), "GPIO CTRL1 not ready");

	// These will be used for gpio emul
	zassert_true(device_is_ready(sensor_output1.boost_en.port), "GPIO EN1 not ready");
	zassert_true(device_is_ready(sensor_output1.boost_ctrl1.port), "GPIO CTRL1 not ready");
	zassert_true(device_is_ready(sensor_output1.boost_ctrl2.port), "GPIO CTRL2 not ready");

	zassert_true(device_is_ready(sensor_output2.boost_en.port), "GPIO EN2 not ready");
	zassert_true(device_is_ready(sensor_output2.boost_ctrl1.port), "GPIO CTRL1 not ready");
	zassert_true(device_is_ready(sensor_output2.boost_ctrl2.port), "GPIO CTRL2 not ready");

	zassert_equal(regulator_fake_set_voltage_fake.call_count, 2, "regulator_set_voltage not called twice");
	// Check the first regulator was set up correctly 
	zassert_equal(regulator_fake_set_voltage_fake.arg0_history[0], sensor_output1.ldo_dev);
	zassert_equal(regulator_fake_set_voltage_fake.arg1_history[0], 3300000);
	zassert_equal(regulator_fake_set_voltage_fake.arg2_history[0], 3300000);
	// Check the second regulator was set up correctly
	zassert_equal(regulator_fake_set_voltage_fake.arg0_history[1], sensor_output2.ldo_dev);
	zassert_equal(regulator_fake_set_voltage_fake.arg1_history[1], 3300000);
	zassert_equal(regulator_fake_set_voltage_fake.arg2_history[1], 3300000);

	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_OFF);
	confirm_voltage(&sensor_output2, SENSOR_VOLTAGE_OFF);
}

/**
 * @brief Call after each test
 * 
 */
static void after_tests(void)
{
	// Reset all voltages to OFF after each test
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_OFF);
	set_sensor_output(&sensor_output2, SENSOR_VOLTAGE_OFF);
}

/**
 * @brief Call before each test
 * 
 */
static void before_tests(void)
{
	reset_all_fakes();
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
ZTEST_SUITE(sensor_power_tests, NULL, testsuite_setup, before_tests, after_tests, NULL);

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output OFF
 * 
 */
ZTEST(sensor_power_tests, test_set_regulator_off)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_OFF);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_OFF);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 3.3V
 * 
 */
ZTEST(sensor_power_tests, test_set_regulator_3v3)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 5V
 * 
 */
ZTEST(sensor_power_tests, test_set_regulator_5v)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_5V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_5V);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 6V
 * 
 */
ZTEST(sensor_power_tests, test_set_regulator_6v)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_6V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_6V);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 12V
 * 
 */
ZTEST(sensor_power_tests, test_set_regulator_12v)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_12V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_12V);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 24V
 * 
 */
ZTEST(sensor_power_tests, test_set_regulator_24v)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_24V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_24V);
}

/**
 * @brief Confirm that multiple outputs can be set correctly 
 * 
 */
ZTEST(sensor_power_tests, test_set_different_sensors_different_voltages)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_6V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_6V);

	set_sensor_output(&sensor_output2, SENSOR_VOLTAGE_24V);
	confirm_voltage(&sensor_output2, SENSOR_VOLTAGE_24V);
}

/**
 * @brief Confirm that the same output can be set correctly multiple times
 * 
 */
ZTEST(sensor_power_tests, test_set_same_sensor_multiple_times)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);

	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_OFF);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_OFF);
	zassert_equal(regulator_fake_disable_fake.call_count, 1, "Expected regulator_disable to be called once when going from 3V3 to OFF");
}

/**
 * @brief Confirm that invalid outputs set output to OFF
 * 
 */
ZTEST(sensor_power_tests, test_invalid_output_sets_voltage_off)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);

	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_INDEX_LIMIT);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_OFF);
}


/**
 * @brief Get correct name associated to a sensor value
 * 
 */
ZTEST(sensor_power_tests, test_get_sensor_output_name)
{
	char voltage_name[20];
	char * expected_name = "SENSOR_VOLTAGE_3V3";
	int ret = get_sensor_voltage_name(voltage_name, SENSOR_VOLTAGE_3V3);
	zassert_ok(ret, "Issue getting the correct name");
	zassert_str_equal(voltage_name, expected_name, "%s is not %s", voltage_name, expected_name);
}

ZTEST(sensor_power_tests, test_sensor_output_name_out_of_bounds)
{
	char voltage_name[20];
	int ret = get_sensor_voltage_name(voltage_name, SENSOR_VOLTAGE_INDEX_LIMIT);
	zassert_not_ok(ret, "did not detect out of bounds name");
	// zassert_str_equal(voltage_name, expected_name, "%s is not %s", voltage_name, expected_name);
}

/**
 * @brief Confirm that sensor output read adc is working 
 * 
 */
ZTEST(sensor_power_tests, test_output_read_expected_value)
{
	float accepted_error = 0.05; // Take into account integer rounding errors in the conversion
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);
	const uint16_t input_mv = 2500;
	const float expected_output = (float)(input_mv / 1000.0) * (float)(113.0/13.0);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, input_mv);
	float output = read_sensor_output(&sensor_output1);
	zassert_within(output, expected_output, accepted_error, "Mismatch: got %f, expected %f", output, expected_output);
}

ZTEST(sensor_power_tests, test_validate_output_3v3)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	uint8_t allowed_error = 5;
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_3V3, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}


