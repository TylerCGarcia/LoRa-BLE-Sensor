/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - Sensor power initialization
 * - All regulators are off after initializaiton
 * - Regulators are less than 3V when off
 * - A regulators current settings can be read
 * - A regulator can be set to 3.3V
 * - A regulator can be set to 5V
 * - A regulator can be set to 6V
 * - A regulator can be set to 12V
 * - A regulator can be set to 24V
 * - The ADC reads the correct regulator voltage 
 */

#include <zephyr/fff.h>
#include <zephyr/ztest.h>
#include "sensor_power.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/regulator/fake.h>

DEFINE_FFF_GLOBALS;

sensor_power_config_t sensor_output1 = {
	.output_id = SENSOR_OUTPUT_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1))
};

sensor_power_config_t sensor_output2 = {
	.output_id = SENSOR_OUTPUT_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2))
};

struct {
	const struct device *en_dev;
	const struct device *ctrl1_dev;
	const struct device *ctrl2_dev;
	gpio_pin_t en_pin;
	gpio_pin_t ctrl1_pin;
	gpio_pin_t ctrl2_pin;
} s1_gpio, s2_gpio;

void testsuite_setup(void)
{
	RESET_FAKE(regulator_fake_set_voltage);
	RESET_FAKE(regulator_fake_enable);
	RESET_FAKE(regulator_fake_disable);

	int err;
	err = sensor_power_init(&sensor_output1);
	zassert_ok(err, "Sensor power initialization failed");

	err = sensor_power_init(&sensor_output2);
	zassert_ok(err, "Sensor power initialization failed");
	// These will be used for gpio emul
	s1_gpio.en_dev = sensor_output1.boost_en.port;
	s1_gpio.en_pin = sensor_output1.boost_en.pin;

	s1_gpio.ctrl1_dev = sensor_output1.boost_ctrl1.port;
	s1_gpio.ctrl1_pin = sensor_output1.boost_ctrl1.pin;

	s1_gpio.ctrl2_dev = sensor_output1.boost_ctrl2.port;
	s1_gpio.ctrl2_pin = sensor_output1.boost_ctrl2.pin;

	s2_gpio.en_dev = sensor_output2.boost_en.port;
	s2_gpio.en_pin = sensor_output2.boost_en.pin;

	s2_gpio.ctrl1_dev = sensor_output2.boost_ctrl1.port;
	s2_gpio.ctrl1_pin = sensor_output2.boost_ctrl1.pin;

	s2_gpio.ctrl2_dev = sensor_output2.boost_ctrl2.port;
	s2_gpio.ctrl2_pin = sensor_output2.boost_ctrl2.pin;

	zassert_true(device_is_ready(s1_gpio.en_dev), "GPIO EN1 not ready");
	zassert_true(device_is_ready(s1_gpio.ctrl1_dev), "GPIO CTRL1 not ready");
	zassert_true(device_is_ready(s1_gpio.ctrl2_dev), "GPIO CTRL2 not ready");

	zassert_true(device_is_ready(s2_gpio.en_dev), "GPIO EN2 not ready");
	zassert_true(device_is_ready(s2_gpio.ctrl1_dev), "GPIO CTRL1 not ready");
	zassert_true(device_is_ready(s2_gpio.ctrl2_dev), "GPIO CTRL2 not ready");

	zassert_equal(regulator_fake_disable_fake.call_count, 2, "Expected regulator_disable to be called twice");
	zassert_equal(regulator_fake_set_voltage_fake.call_count, 2, "regulator_set_voltage not called twice");
	// Check the first regulator was set up correctly 
	zassert_equal(regulator_fake_set_voltage_fake.arg0_history[0], sensor_output1.ldo_dev);
	zassert_equal(regulator_fake_set_voltage_fake.arg1_history[0], 3300000);
	zassert_equal(regulator_fake_set_voltage_fake.arg2_history[0], 3300000);
	// Check the second regulator was set up correctly
	zassert_equal(regulator_fake_set_voltage_fake.arg0_history[1], sensor_output2.ldo_dev);
	zassert_equal(regulator_fake_set_voltage_fake.arg1_history[1], 3300000);
	zassert_equal(regulator_fake_set_voltage_fake.arg2_history[1], 3300000);
}

void before_tests(void)
{
	// Clear previous call history
	RESET_FAKE(regulator_fake_set_voltage);
	RESET_FAKE(regulator_fake_enable);
	RESET_FAKE(regulator_fake_disable);
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
ZTEST_SUITE(sensor_power_tests, NULL, testsuite_setup, before_tests, NULL, NULL);

/**
 * @brief Tests that both sensor outputs are OFF after initialization.
 * 
 */
ZTEST(sensor_power_tests, test_regulators_off_after_init)
{
	int sensor_state = get_sensor_voltage(&sensor_output1);
	zassert_equal(sensor_state, SENSOR_VOLTAGE_OFF, "Sensor output 1 should be off");
	int sensor_state2 = get_sensor_voltage(&sensor_output2);
	zassert_equal(sensor_state2, SENSOR_VOLTAGE_OFF, "Sensor output 2 should be off");
}

ZTEST(sensor_power_tests, test_set_regulator_3V3)
{
	set_sensor_voltage(&sensor_output1, SENSOR_VOLTAGE_3V3);

	zassert_equal(regulator_fake_enable_fake.call_count, 1, "Expected regulator_enable to be called once");

	int val_en = gpio_emul_output_get(s1_gpio.en_dev, s1_gpio.en_pin);
	int val_ctrl1 = gpio_emul_output_get(s1_gpio.ctrl1_dev, s1_gpio.ctrl1_pin);
	int val_ctrl2 = gpio_emul_output_get(s1_gpio.ctrl2_dev, s1_gpio.ctrl2_pin);

    zassert_equal(val_en, 0, "BOOST_EN should be LOW for 3V3");
    zassert_equal(val_ctrl1, 0, "CTRL1 should be LOW for 3V3");
    zassert_equal(val_ctrl2, 0, "CTRL2 should be LOW for 3V3");
}

ZTEST(sensor_power_tests, test_set_regulator_5V)
{
	set_sensor_voltage(&sensor_output1, SENSOR_VOLTAGE_5V);

	int val_en = gpio_emul_output_get(s1_gpio.en_dev, s1_gpio.en_pin);
	int val_ctrl1 = gpio_emul_output_get(s1_gpio.ctrl1_dev, s1_gpio.ctrl1_pin);
	int val_ctrl2 = gpio_emul_output_get(s1_gpio.ctrl2_dev, s1_gpio.ctrl2_pin);

    zassert_equal(val_en, 1, "BOOST_EN should be HIGH for 5V");
    zassert_equal(val_ctrl1, 1, "CTRL1 should be HIGH for 5V");
    zassert_equal(val_ctrl2, 1, "CTRL2 should be HIGH for 5V");
}

ZTEST(sensor_power_tests, test_set_regulator_6V)
{
	set_sensor_voltage(&sensor_output1, SENSOR_VOLTAGE_6V);

	int val_en = gpio_emul_output_get(s1_gpio.en_dev, s1_gpio.en_pin);
	int val_ctrl1 = gpio_emul_output_get(s1_gpio.ctrl1_dev, s1_gpio.ctrl1_pin);
	int val_ctrl2 = gpio_emul_output_get(s1_gpio.ctrl2_dev, s1_gpio.ctrl2_pin);

    zassert_equal(val_en, 1, "BOOST_EN should be HIGH for 6V");
    zassert_equal(val_ctrl1, 0, "CTRL1 should be LOW for 6V");
    zassert_equal(val_ctrl2, 1, "CTRL2 should be HIGH for 6V");
}

ZTEST(sensor_power_tests, test_set_regulator_12V)
{
	set_sensor_voltage(&sensor_output1, SENSOR_VOLTAGE_12V);

	int val_en = gpio_emul_output_get(s1_gpio.en_dev, s1_gpio.en_pin);
	int val_ctrl1 = gpio_emul_output_get(s1_gpio.ctrl1_dev, s1_gpio.ctrl1_pin);
	int val_ctrl2 = gpio_emul_output_get(s1_gpio.ctrl2_dev, s1_gpio.ctrl2_pin);

    zassert_equal(val_en, 1, "BOOST_EN should be HIGH for 12V");
    zassert_equal(val_ctrl1, 1, "CTRL1 should be HIGH for 12V");
    zassert_equal(val_ctrl2, 0, "CTRL2 should be LOW for 12V");
}

ZTEST(sensor_power_tests, test_set_regulator_24V)
{
	set_sensor_voltage(&sensor_output1, SENSOR_VOLTAGE_24V);

	int val_en = gpio_emul_output_get(s1_gpio.en_dev, s1_gpio.en_pin);
	int val_ctrl1 = gpio_emul_output_get(s1_gpio.ctrl1_dev, s1_gpio.ctrl1_pin);
	int val_ctrl2 = gpio_emul_output_get(s1_gpio.ctrl2_dev, s1_gpio.ctrl2_pin);

    zassert_equal(val_en, 1, "BOOST_EN should be HIGH for 24V");
    zassert_equal(val_ctrl1, 0, "CTRL1 should be LOW for 24V");
    zassert_equal(val_ctrl2, 0, "CTRL2 should be LOW for 24V");
}

ZTEST(sensor_power_tests, test_set_sensors_different_voltages)
{
	set_sensor_voltage(&sensor_output1, SENSOR_VOLTAGE_6V);
	set_sensor_voltage(&sensor_output2, SENSOR_VOLTAGE_24V);

	int sensor1_voltage = get_sensor_voltage(&sensor_output1);
	zassert_equal(sensor1_voltage, SENSOR_VOLTAGE_6V, "Sensor output 1 should be 6V");

	int s1_val_en = gpio_emul_output_get(s1_gpio.en_dev, s1_gpio.en_pin);
	int s1_val_ctrl1 = gpio_emul_output_get(s1_gpio.ctrl1_dev, s1_gpio.ctrl1_pin);
	int s1_val_ctrl2 = gpio_emul_output_get(s1_gpio.ctrl2_dev, s1_gpio.ctrl2_pin);


	zassert_equal(s1_val_en, 1, "BOOST_EN should be HIGH for 6V");
    zassert_equal(s1_val_ctrl1, 0, "CTRL1 should be LOW for 6V");
    zassert_equal(s1_val_ctrl2, 1, "CTRL2 should be HIGH for 6V");

	int sensor2_voltage = get_sensor_voltage(&sensor_output2);
	zassert_equal(sensor2_voltage, SENSOR_VOLTAGE_24V, "Sensor output 2 should be 24V");

	int s2_val_en = gpio_emul_output_get(s2_gpio.en_dev, s2_gpio.en_pin);
	int s2_val_ctrl1 = gpio_emul_output_get(s2_gpio.ctrl1_dev, s2_gpio.ctrl1_pin);
	int s2_val_ctrl2 = gpio_emul_output_get(s2_gpio.ctrl2_dev, s2_gpio.ctrl2_pin);

    zassert_equal(s2_val_en, 1, "BOOST_EN should be HIGH for 24V");
    zassert_equal(s2_val_ctrl1, 0, "CTRL1 should be LOW for 24V");
    zassert_equal(s2_val_ctrl2, 0, "CTRL2 should be LOW for 24V");
}