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
	.power_id = SENSOR_POWER_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output1),
	.delay_ms = 100
};

sensor_power_config_t sensor_output2 = {
	.power_id = SENSOR_POWER_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output2),
	.delay_ms = 100
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
static void *testsuite_setup(void)
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
static void *after_tests(void)
{
	// Reset all voltages to OFF after each test
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_OFF);
	set_sensor_output(&sensor_output2, SENSOR_VOLTAGE_OFF);
}

/**
 * @brief Call before each test
 * 
 */
static void *before_tests(void)
{
	reset_all_fakes();
}

/**
 * @brief Test Sensor Power
 *
 * This test suite verifies the sensor power system is working correctly.
 *
 */
ZTEST_SUITE(power, NULL, testsuite_setup, before_tests, after_tests, NULL);

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output OFF
 * 
 */
ZTEST(power, test_set_regulator_off)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_OFF);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_OFF);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 3.3V
 * 
 */
ZTEST(power, test_set_regulator_3v3)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 5V
 * 
 */
ZTEST(power, test_set_regulator_5v)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_5V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_5V);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 6V
 * 
 */
ZTEST(power, test_set_regulator_6v)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_6V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_6V);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 12V
 * 
 */
ZTEST(power, test_set_regulator_12v)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_12V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_12V);
}

/**
 * @brief Confirm the correct gpios are set and regulator calls are made for setting an output to 24V
 * 
 */
ZTEST(power, test_set_regulator_24v)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_24V);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_24V);
}

/**
 * @brief Confirm that multiple outputs can be set correctly 
 * 
 */
ZTEST(power, test_set_different_sensors_different_voltages)
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
ZTEST(power, test_set_same_sensor_multiple_times)
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
ZTEST(power, test_invalid_output_sets_voltage_off)
{
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);

	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_INDEX_LIMIT);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_OFF);
}

/**
 * @brief Confirm that sensor output read adc is working 
 * 
 */
ZTEST(power, test_output_read_expected_value)
{
	float accepted_error = 0.05; // Take into account integer rounding errors in the conversion
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);
	const uint16_t input_mv = 2500;
	const float expected_output = (float)(input_mv / 1000.0) * (float)(((float)OUTUT_READ_DIVIDER_HIGH + (float)OUTUT_READ_DIVIDER_LOW)/(float)OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, input_mv);
	float output = read_sensor_output(&sensor_output1);
	zassert_within(output, expected_output, accepted_error, "Mismatch: got %f, expected %f", output, expected_output);
}

/**
 * @brief Test validate_output for 3v3, making sure valid adc reading and sensor was set correctly
 * 
 */
ZTEST(power, test_validate_output_3v3)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);

	uint8_t allowed_error = 5;
	const uint16_t input_mv = 3300; // 3.3V
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_3V3, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output for 5V, making sure valid adc reading and sensor was set correctly
 * 
 */
ZTEST(power, test_validate_output_5v)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_5V);

	uint8_t allowed_error = 5;
	const uint16_t input_mv = 5000; // 5V
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_5V, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output for 6V, making sure valid adc reading and sensor was set correctly
 * 
 */
ZTEST(power, test_validate_output_6v)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_6V);

	uint8_t allowed_error = 5;
	const uint16_t input_mv = 6000; // 6V
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_6V, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output for 12V, making sure valid adc reading and sensor was set correctly
 * 
 */
ZTEST(power, test_validate_output_12v)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_12V);

	uint8_t allowed_error = 5;
	const uint16_t input_mv = 12000; // 12V
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_12V, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output for 24V, making sure valid adc reading and sensor was set correctly
 * 
 */
ZTEST(power, test_validate_output_24v)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_24V);

	uint8_t allowed_error = 5;
	const uint16_t input_mv = 24000; // 24V
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_24V, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output has error for 3v3 when there is a 6% input error above and accepted_error is set to 5%
 * 
 */
ZTEST(power, test_validate_output_3v3_out_of_bounds_high)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 3498; // 3.3V + 6% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_3V3, allowed_error);
	zassert_not_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output has error for 3v3 when there is a 6% input error below and accepted_error is set to 5%
 * 
 */
ZTEST(power, test_validate_output_3v3_out_of_bounds_low)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 3102; // 3.3V - 6% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_3V3, allowed_error);
	zassert_not_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output for 3v3 when there is a 3% error above and accepted_error is set to 5%
 * 
 */
ZTEST(power, test_validate_output_3v3_in_bounds_high)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 3399; // 3.3V + 3% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_3V3, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output for 3v3 when there is a 3% error below and accepted_error is set to 5%
 * 
 */
ZTEST(power, test_validate_output_3v3_in_bounds_low)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 3201; // 3.3V - 3% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_3V3, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}


/**
 * @brief Test validate_output has error for 24v when there is a 6% input error above and accepted_error is set to 5%
 * 
 */
ZTEST(power, test_validate_output_24v_out_of_bounds_high)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_24V);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 25440; // 24V + 6% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_24V, allowed_error);
	zassert_not_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output has error for 24v when there is a 6% input error below and accepted_error is set to 5%
 * 
 */
ZTEST(power, test_validate_output_24v_out_of_bounds_low)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_24V);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 22560; // 24V - 6% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_24V, allowed_error);
	zassert_not_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output for 24V when there is a 3% error above and accepted_error is set to 5%
 * 
 */
ZTEST(power, test_validate_output_24v_in_bounds_high)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_24V);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 24720; // 24V + 3% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_24V, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output for 24v when there is a 3% error below and accepted_error is set to 5%
 * 
 */
ZTEST(power, test_validate_output_24v_in_bounds_low)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_24V);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 23280; // 24V - 3% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_24V, allowed_error);
	zassert_ok(ret, "Voltage read does not match expected value");
}

/**
 * @brief Test validate_output when the voltage read is correct but the set voltage is incorrect, should produce an error
 * 
 */
ZTEST(power, test_validate_output_wrong_setting)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	
	uint8_t allowed_error = 5;
	const uint16_t input_mv = 24000; // 24V - 3% error
	const uint16_t emul_mv = (input_mv * OUTUT_READ_DIVIDER_LOW) / (OUTUT_READ_DIVIDER_HIGH + OUTUT_READ_DIVIDER_LOW);
	adc_emul_const_value_set(sensor_output1.output_read.dev, sensor_output1.output_read.channel_id, emul_mv);
	
	ret = validate_output(&sensor_output1, SENSOR_VOLTAGE_24V, allowed_error);
	zassert_not_ok(ret, "Voltage does not match expected value");
}

/**
 * @brief Test that the output is off before setting a voltage. Will not set voltage correctly if being set
 * from a different on voltage.
 * 
 */
ZTEST(power, test_sets_output_off_before_setting_voltage)
{
	int ret;
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);
	set_sensor_output(&sensor_output1, SENSOR_VOLTAGE_3V3);
	confirm_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);

	// the second set sensor output should call regulator_disable once to turn off before setting the second call
	zassert_equal(regulator_fake_disable_fake.call_count, 1, "regulator_disable should be %d, but was %d", 3, regulator_fake_set_voltage_fake.call_count);
}
