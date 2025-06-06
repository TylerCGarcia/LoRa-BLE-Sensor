/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 * Tests:
 * - Get correct name associated to a sensor value for Voltage OFF
 * - Get correct name associated to a sensor value for 3V3
 * - Get correct name associated to a sensor value for 5V
 * - Get correct name associated to a sensor value for 6V
 * - Get correct name associated to a sensor value for 12V
 * - Get correct name associated to a sensor value for 24V
 * - Get correct index from the name for SENSOR_VOLTAGE_OFF
 */

#include <zephyr/ztest.h>
#include "sensor_names.h"

/**
 * @brief Test Sensor Power
 *
 * This test suite verifies the sensor power system is working correctly.
 *
 */
ZTEST_SUITE(names, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Get correct name associated to a sensor value for Voltage OFF
 * 
 */
ZTEST(names, test_get_sensor_output_name_from_index_off)
{
	char voltage_name[20];
	char * expected_name = "SENSOR_VOLTAGE_OFF";
	int ret = get_sensor_voltage_name_from_index(voltage_name, SENSOR_VOLTAGE_OFF);
	zassert_ok(ret, "Issue getting the correct name");
	zassert_str_equal(voltage_name, expected_name, "%s is not %s", voltage_name, expected_name);
}

/**
 * @brief Get correct name associated to a sensor value for 3v3
 * 
 */
ZTEST(names, test_get_sensor_output_name_from_index_3v3)
{
	char voltage_name[20];
	char * expected_name = "SENSOR_VOLTAGE_3V3";
	int ret = get_sensor_voltage_name_from_index(voltage_name, SENSOR_VOLTAGE_3V3);
	zassert_ok(ret, "Issue getting the correct name");
	zassert_str_equal(voltage_name, expected_name, "%s is not %s", voltage_name, expected_name);
}

/**
 * @brief Get correct name associated to a sensor value for 5V
 * 
 */
ZTEST(names, test_get_sensor_output_name_from_index_5v)
{
	char voltage_name[20];
	char * expected_name = "SENSOR_VOLTAGE_5V";
	int ret = get_sensor_voltage_name_from_index(voltage_name, SENSOR_VOLTAGE_5V);
	zassert_ok(ret, "Issue getting the correct name");
	zassert_str_equal(voltage_name, expected_name, "%s is not %s", voltage_name, expected_name);
}

/**
 * @brief Get correct name associated to a sensor value for 5V
 * 
 */
ZTEST(names, test_get_sensor_output_name_from_index_6v)
{
	char voltage_name[20];
	char * expected_name = "SENSOR_VOLTAGE_6V";
	int ret = get_sensor_voltage_name_from_index(voltage_name, SENSOR_VOLTAGE_6V);
	zassert_ok(ret, "Issue getting the correct name");
	zassert_str_equal(voltage_name, expected_name, "%s is not %s", voltage_name, expected_name);
}

/**
 * @brief Get correct name associated to a sensor value for 12V
 * 
 */
ZTEST(names, test_get_sensor_output_name_from_index_12v)
{
	char voltage_name[20];
	char * expected_name = "SENSOR_VOLTAGE_12V";
	int ret = get_sensor_voltage_name_from_index(voltage_name, SENSOR_VOLTAGE_12V);
	zassert_ok(ret, "Issue getting the correct name");
	zassert_str_equal(voltage_name, expected_name, "%s is not %s", voltage_name, expected_name);
}

/**
 * @brief Get correct name associated to a sensor value for 24V
 * 
 */
ZTEST(names, test_get_sensor_output_name_from_index_24v)	
{
	char voltage_name[20];
	char * expected_name = "SENSOR_VOLTAGE_24V";
	int ret = get_sensor_voltage_name_from_index(voltage_name, SENSOR_VOLTAGE_24V);
	zassert_ok(ret, "Issue getting the correct name");
	zassert_str_equal(voltage_name, expected_name, "%s is not %s", voltage_name, expected_name);
}

/**
 * @brief Test upper and lower bounds return of get_sensor_voltage_name_from_index for out of bounds value too high
 * 
 */
ZTEST(names, test_get_sensor_output_name_from_index_out_of_bounds_high)
{
	char voltage_name[20];
	int ret = get_sensor_voltage_name_from_index(voltage_name, SENSOR_VOLTAGE_INDEX_LIMIT);
	zassert_not_ok(ret, "did not detect out of bounds name");
}

/**
 * @brief Test getting the name of the sensor type from the index for an out of bounds sensor type value too low
 * 
 */
ZTEST(names, test_get_sensor_output_name_from_index_out_of_bounds_low)
{
	char voltage_name[20];
	int ret = get_sensor_voltage_name_from_index(voltage_name, -1);
	zassert_not_ok(ret, "did not detect out of bounds name");
}

/**
 * @brief Test getting the index from the name for SENSOR_VOLTAGE_OFF
 * 
 */
ZTEST(names,test_get_sensor_output_index_from_name_off)
{
	char voltage_name[20];
	strcpy(voltage_name, "SENSOR_VOLTAGE_OFF");
	int ret_index = get_sensor_voltage_index_from_name(voltage_name);
	zassert_equal(ret_index, SENSOR_VOLTAGE_OFF, "Expected SENSOR_VOLTAGE_OFF");
}

/**
 * @brief Test getting the index from the name for SENSOR_VOLTAGE_3V3
 * 
 */
ZTEST(names,test_get_sensor_output_index_from_name_3v3)
{
	char voltage_name[20];
	strcpy(voltage_name, "SENSOR_VOLTAGE_3V3");
	int ret_index = get_sensor_voltage_index_from_name(voltage_name);
	zassert_equal(ret_index, SENSOR_VOLTAGE_3V3, "Expected SENSOR_VOLTAGE_3V3");
}

/**
 * @brief Test getting the index from the name for SENSOR_VOLTAGE_5V
 * 
 */
ZTEST(names,test_get_sensor_output_index_from_name_5v)
{
	char voltage_name[20];
	strcpy(voltage_name, "SENSOR_VOLTAGE_5V");
	int ret_index = get_sensor_voltage_index_from_name(voltage_name);
	zassert_equal(ret_index, SENSOR_VOLTAGE_5V, "Expected SENSOR_VOLTAGE_5V");
}

/**
 * @brief Test getting the index from the name for SENSOR_VOLTAGE_6V
 * 
 */
ZTEST(names,test_get_sensor_output_index_from_name_6v)
{
	char voltage_name[20];
	strcpy(voltage_name, "SENSOR_VOLTAGE_6V");
	int ret_index = get_sensor_voltage_index_from_name(voltage_name);
	zassert_equal(ret_index, SENSOR_VOLTAGE_6V, "Expected SENSOR_VOLTAGE_6V");
}	

/**
 * @brief Test getting the index from the name for SENSOR_VOLTAGE_12V
 * 
 */
ZTEST(names,test_get_sensor_output_index_from_name_12v)
{	
	char voltage_name[20];
	strcpy(voltage_name, "SENSOR_VOLTAGE_12V");
	int ret_index = get_sensor_voltage_index_from_name(voltage_name);
	zassert_equal(ret_index, SENSOR_VOLTAGE_12V, "Expected SENSOR_VOLTAGE_12V");
}

/**
 * @brief Test getting the index from the name for SENSOR_VOLTAGE_24V
 * 
 */
ZTEST(names,test_get_sensor_output_index_from_name_24v)
{
	char voltage_name[20];
	strcpy(voltage_name, "SENSOR_VOLTAGE_24V");
	int ret_index = get_sensor_voltage_index_from_name(voltage_name);
	zassert_equal(ret_index, SENSOR_VOLTAGE_24V, "Expected SENSOR_VOLTAGE_24V");
}

/**
 * @brief Test getting the index from the name for an invalid name
 * 
 */
ZTEST(names,test_get_sensor_output_index_from_name_invalid)
{
	char voltage_name[20];
	strcpy(voltage_name, "INVALID");
	int ret_index = get_sensor_voltage_index_from_name(voltage_name);
	zassert_equal(ret_index, -1, "Expected invalid name to return -1");
}

/**
 * @brief Test getting the name of the sensor type from the index for NULL_SENSOR
 * 
 */
ZTEST(names, test_get_sensor_type_name_from_index_null_sensor)
{
	char sensor_type_name[20];
	char * expected_name = "NULL_SENSOR";
	int ret = get_sensor_type_name_from_index(sensor_type_name, NULL_SENSOR);
	zassert_str_equal(sensor_type_name, expected_name, "%s is not %s", sensor_type_name, expected_name);
}

/**
 * @brief Test getting the name of the sensor type from the index for VOLTAGE_SENSOR
 * 
 */
ZTEST(names, test_get_sensor_type_name_from_index_voltage_sensor)
{
	char sensor_type_name[20];
	char * expected_name = "VOLTAGE_SENSOR";
	int ret = get_sensor_type_name_from_index(sensor_type_name, VOLTAGE_SENSOR);
	zassert_str_equal(sensor_type_name, expected_name, "%s is not %s", sensor_type_name, expected_name);
}

/**
 * @brief Test getting the name of the sensor type from the index for CURRENT_SENSOR
 * 
 */
ZTEST(names, test_get_sensor_type_name_from_index_current_sensor)
{
	char sensor_type_name[20];
	char * expected_name = "CURRENT_SENSOR";
	int ret = get_sensor_type_name_from_index(sensor_type_name, CURRENT_SENSOR);
	zassert_str_equal(sensor_type_name, expected_name, "%s is not %s", sensor_type_name, expected_name);
}

/**
 * @brief Test getting the name of the sensor type from the index for PULSE_SENSOR
 * 
 */
ZTEST(names, test_get_sensor_type_name_from_index_pulse_sensor)
{
	char sensor_type_name[20];
	char * expected_name = "PULSE_SENSOR";
	int ret = get_sensor_type_name_from_index(sensor_type_name, PULSE_SENSOR);
	zassert_str_equal(sensor_type_name, expected_name, "%s is not %s", sensor_type_name, expected_name);
}

/**
 * @brief Test getting the name of the sensor type from the index for an out of bounds sensor type value too high
 * 
 */
ZTEST(names, test_get_sensor_type_name_from_index_out_of_bounds_high)
{
	char sensor_type_name[20];
	int ret = get_sensor_type_name_from_index(sensor_type_name, SENSOR_TYPE_LIMIT);
	zassert_not_ok(ret, "Expected out of bounds sensor type to return -1");
}

/**
 * @brief Test getting the name of the sensor type from the index for an out of bounds sensor type value too low
 * 
 */
ZTEST(names, test_get_sensor_type_name_from_index_out_of_bounds_low)
{
	char sensor_type_name[20];
	int ret = get_sensor_type_name_from_index(sensor_type_name, -1);
	zassert_not_ok(ret, "Expected out of bounds sensor type to return -1");
}

/**
 * @brief Test getting the sensor type index from the name for NULL_SENSOR
 * 
 */
ZTEST(names, test_get_sensor_type_index_from_name_null_sensor)
{
	char sensor_type_name[20];
	strcpy(sensor_type_name, "NULL_SENSOR");
	int ret_index = get_sensor_type_index_from_name(sensor_type_name);
	zassert_equal(ret_index, NULL_SENSOR, "Expected NULL_SENSOR");
}

/**
 * @brief Test getting the sensor type index from the name for VOLTAGE_SENSOR
 * 
 */
ZTEST(names, test_get_sensor_type_index_from_name_voltage_sensor)
{
	char sensor_type_name[20];
	strcpy(sensor_type_name, "VOLTAGE_SENSOR");
	int ret_index = get_sensor_type_index_from_name(sensor_type_name);
	zassert_equal(ret_index, VOLTAGE_SENSOR, "Expected VOLTAGE_SENSOR");
}

/**
 * @brief Test getting the sensor type index from the name for CURRENT_SENSOR
 * 
 */
ZTEST(names, test_get_sensor_type_index_from_name_current_sensor)
{
	char sensor_type_name[20];
	strcpy(sensor_type_name, "CURRENT_SENSOR");
	int ret_index = get_sensor_type_index_from_name(sensor_type_name);
	zassert_equal(ret_index, CURRENT_SENSOR, "Expected CURRENT_SENSOR");
}

/**
 * @brief Test getting the sensor type index from the name for PULSE_SENSOR
 * 
 */
ZTEST(names, test_get_sensor_type_index_from_name_pulse_sensor)
{
	char sensor_type_name[20];
	strcpy(sensor_type_name, "PULSE_SENSOR");
	int ret_index = get_sensor_type_index_from_name(sensor_type_name);
	zassert_equal(ret_index, PULSE_SENSOR, "Expected PULSE_SENSOR");
}

/**
 * @brief Test getting the sensor type index from the name for an invalid name
 * 
 */
ZTEST(names, test_get_sensor_type_index_from_name_invalid)
{
	char sensor_type_name[20];
	strcpy(sensor_type_name, "INVALID");
	int ret_index = get_sensor_type_index_from_name(sensor_type_name);
	zassert_equal(ret_index, -1, "Expected invalid name to return -1");
}