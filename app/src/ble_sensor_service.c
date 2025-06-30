/**
 * @file ble_sensor_service.c
 * @author Tyler Garcia
 * @brief This is a library to handle the BLE services for the sensor.
 * It allows for configurations of the sensor, including power configuration, 
 * sensor configuration, and data frequency.
 * It also allows for the retrieval of the sensor data.
 * 
 * @version 0.1
 * @date 2025-05-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ble_sensor_service.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>
#include "sensor_nvs.h"
#include "sensor_names.h"

LOG_MODULE_REGISTER(BLE_SENSOR_SERVICE, LOG_LEVEL_DBG);

static sensor_app_config_t *sensor_app_config;
static int is_sensor_service_setup = 0;

static ssize_t read_sensor_state(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}
    uint8_t sensor_state = sensor_app_config->state;
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_state, sizeof(sensor_state));
}

static ssize_t write_sensor_state(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    uint8_t sensor_state = 0;
    LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

    // Check bounds of data
	if (len != sizeof(sensor_state)) {
		LOG_ERR("Write date: Data length incorrect for sensor state");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for sensor state");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into buffer
	memcpy(&sensor_state, buf, len);

	LOG_INF("Sensor state RECEIVED: %d", sensor_state);
    sensor_app_config->state = sensor_state;
    sensor_nvs_write(SENSOR_NVS_ADDRESS_APP_STATE, &sensor_app_config->state, sizeof(sensor_app_config->state));
	return len;
}

static ssize_t read_sensor1_enabled(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_app_config->is_sensor_1_enabled, sizeof(sensor_app_config->is_sensor_1_enabled));
}

static ssize_t write_sensor1_enabled(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(sensor_app_config->is_sensor_1_enabled)) {
		LOG_ERR("Write date: Data length incorrect for sensor 1 enabled");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for sensor 1 enabled");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into lorawan enabled buffer
	memcpy(&sensor_app_config->is_sensor_1_enabled, buf, len);

	LOG_INF("Sensor 1 enabled new value: %d", sensor_app_config->is_sensor_1_enabled);
    sensor_nvs_write(SENSOR_NVS_ADDRESS_SENSOR_1_ENABLED, &sensor_app_config->is_sensor_1_enabled, sizeof(sensor_app_config->is_sensor_1_enabled));
	return len;
}

static ssize_t read_sensor1_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}
	return bt_gatt_attr_read(conn, attr, buf, len, offset, sensor_app_config->sensor_1_type_name, sizeof(sensor_app_config->sensor_1_type_name));
}

static ssize_t write_sensor1_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    char sensor_1_config_name[SENSOR_TYPE_NAME_LENGTH];
    LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

    // Check bounds of data
	if (len > sizeof(sensor_1_config_name)) {
		LOG_ERR("Write date: Data length incorrect for sensor 1 config");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for sensor 1 config");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into buffer
	memcpy(sensor_1_config_name, buf, len);
	sensor_1_config_name[len] = '\0';  // Add null terminator to end of string
	LOG_INF("Sensor 1 config RECEIVED: %s", sensor_1_config_name);

	// Get index of sensor type from name and check if it is valid
    int ret_index = get_sensor_type_index_from_name(sensor_1_config_name);
	if(ret_index < 0)
	{
		LOG_ERR("Invalid sensor 1 config");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	// Store index of sensor type to config
    sensor_app_config->sensor_1_type = ret_index;
	// Copy name of sensor type to config
	strcpy(sensor_app_config->sensor_1_type_name, sensor_1_config_name);
	// Store index of sensor type to nvs
    sensor_nvs_write(SENSOR_NVS_ADDRESS_SENSOR_1_TYPE, &sensor_app_config->sensor_1_type, sizeof(sensor_app_config->sensor_1_type));
	return len;
}

static ssize_t read_sensor1_pwr_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_app_config->sensor_1_voltage_name, sizeof(sensor_app_config->sensor_1_voltage_name));
}

static ssize_t write_sensor1_pwr_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    char sensor_1_voltage_name[SENSOR_VOLTAGE_NAME_LENGTH];
    LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

    // Check bounds of data
	if (len > sizeof(sensor_1_voltage_name)) {
		LOG_ERR("Write date: Data length incorrect for Sensor 1 Voltage");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for Sensor 1 Voltage");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into buffer
	memcpy(sensor_1_voltage_name, buf, len);
	sensor_1_voltage_name[len] = '\0';  // Add null terminator to end of string

	LOG_INF("Sensor 1 Voltage RECEIVED: %s", sensor_1_voltage_name);
	// Get index of output voltage from name and check if it is valid
    int ret_index = get_sensor_voltage_index_from_name(sensor_1_voltage_name);
	if(ret_index < 0)
	{
		LOG_ERR("Invalid sensor 1 voltage");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	// Store index of output voltage to config
	sensor_app_config->sensor_1_voltage = ret_index;
	// Copy name of output voltage to config
	strcpy(sensor_app_config->sensor_1_voltage_name, sensor_1_voltage_name);
	// Store index of output voltage to nvs
    sensor_nvs_write(SENSOR_NVS_ADDRESS_SENSOR_1_POWER, &sensor_app_config->sensor_1_voltage, strlen(sensor_app_config->sensor_1_voltage_name));
	return len;
}

static ssize_t read_sensor1_data_freq(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_app_config->sensor_1_frequency, sizeof(sensor_app_config->sensor_1_frequency));
}

static ssize_t write_sensor1_data_freq(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(sensor_app_config->sensor_1_frequency)) {
		LOG_ERR("Write date: Data length incorrect for sensor 1 frequency");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for sensor 1 frequency");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into lorawan enabled buffer
	memcpy(&sensor_app_config->sensor_1_frequency, buf, len);

	LOG_INF("Sensor 1 frequency new value: %d", sensor_app_config->sensor_1_frequency);
    sensor_nvs_write(SENSOR_NVS_ADDRESS_SENSOR_1_FREQUENCY, &sensor_app_config->sensor_1_frequency, sizeof(sensor_app_config->sensor_1_frequency));
	return len;
}

static ssize_t read_sensor1_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

    return bt_gatt_attr_read(conn, attr, buf, len, offset, sensor_app_config->sensor_1_latest_data, sizeof(sensor_app_config->sensor_1_latest_data));
}

static ssize_t read_sensor1_data_time(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_app_config->sensor_1_latest_data_timestamp, sizeof(sensor_app_config->sensor_1_latest_data_timestamp));
}

static ssize_t read_sensor2_enabled(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_app_config->is_sensor_2_enabled, sizeof(sensor_app_config->is_sensor_2_enabled));
}

static ssize_t write_sensor2_enabled(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(sensor_app_config->is_sensor_2_enabled)) {
		LOG_ERR("Write date: Data length incorrect for sensor 2 enabled");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for sensor 2 enabled");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into lorawan enabled buffer
	memcpy(&sensor_app_config->is_sensor_2_enabled, buf, len);

	LOG_INF("Sensor 2 enabled new value: %d", sensor_app_config->is_sensor_2_enabled);
    sensor_nvs_write(SENSOR_NVS_ADDRESS_SENSOR_2_ENABLED, &sensor_app_config->is_sensor_2_enabled, sizeof(sensor_app_config->is_sensor_2_enabled));
	return len;
}

static ssize_t read_sensor2_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}
	return bt_gatt_attr_read(conn, attr, buf, len, offset, sensor_app_config->sensor_2_type_name, sizeof(sensor_app_config->sensor_2_type_name));
}

static ssize_t write_sensor2_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    char sensor_2_config_name[SENSOR_TYPE_NAME_LENGTH];
    LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

    // Check bounds of data
	if (len > sizeof(sensor_2_config_name)) {
		LOG_ERR("Write date: Data length incorrect for sensor 2 config");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for sensor 2 config");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into buffer
	memcpy(sensor_2_config_name, buf, len);
	sensor_2_config_name[len] = '\0';  // Add null terminator to end of string
	LOG_INF("Sensor 2 config RECEIVED: %s", sensor_2_config_name);

	// Get index of sensor type from name and check if it is valid
    int ret_index = get_sensor_type_index_from_name(sensor_2_config_name);
	if(ret_index < 0)
	{
		LOG_ERR("Invalid sensor 2 config");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	// Store index of sensor type to config
    sensor_app_config->sensor_2_type = ret_index;
	// Copy name of sensor type to config
	strcpy(sensor_app_config->sensor_2_type_name, sensor_2_config_name);
	// Store index of sensor type to nvs
    sensor_nvs_write(SENSOR_NVS_ADDRESS_SENSOR_2_TYPE, &sensor_app_config->sensor_2_type, sizeof(sensor_app_config->sensor_2_type));
	return len;
}

static ssize_t read_sensor2_pwr_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_app_config->sensor_2_voltage_name, strlen(sensor_app_config->sensor_2_voltage_name));
}

static ssize_t write_sensor2_pwr_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    char sensor_2_voltage_name[SENSOR_VOLTAGE_NAME_LENGTH];
    LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

    // Check bounds of data
	if (len > sizeof(sensor_2_voltage_name)) {
		LOG_ERR("Write date: Data length incorrect for Sensor 2 Voltage");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for Sensor 2 Voltage");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into buffer
	memcpy(sensor_2_voltage_name, buf, len);
	sensor_2_voltage_name[len] = '\0';  // Add null terminator to end of string
	
	LOG_INF("Sensor 2 Voltage RECEIVED: %s", sensor_2_voltage_name);
	// Get index of output voltage from name and check if it is valid
    int ret_index = get_sensor_voltage_index_from_name(sensor_2_voltage_name);
	if(ret_index < 0)
	{
		LOG_ERR("Invalid sensor 2 voltage");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	// Store index of output voltage to config
	sensor_app_config->sensor_2_voltage = ret_index;
	// Copy name of output voltage to config
	strcpy(sensor_app_config->sensor_2_voltage_name, sensor_2_voltage_name);
	// Store index of output voltage to nvs
    sensor_nvs_write(SENSOR_NVS_ADDRESS_SENSOR_2_POWER, &sensor_app_config->sensor_2_voltage, strlen(sensor_app_config->sensor_2_voltage_name));
	return len;
}

static ssize_t read_sensor2_data_freq(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_app_config->sensor_2_frequency, sizeof(sensor_app_config->sensor_2_frequency));
}

static ssize_t write_sensor2_data_freq(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(sensor_app_config->sensor_2_frequency)) {
		LOG_ERR("Write date: Data length incorrect for sensor 2 frequency");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for sensor 2 frequency");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into lorawan enabled buffer
	memcpy(&sensor_app_config->sensor_2_frequency, buf, len);

	LOG_INF("Sensor 2 frequency new value: %d", sensor_app_config->sensor_2_frequency);
    sensor_nvs_write(SENSOR_NVS_ADDRESS_SENSOR_2_FREQUENCY, &sensor_app_config->sensor_2_frequency, sizeof(sensor_app_config->sensor_2_frequency));
	return len;
}

static ssize_t read_sensor2_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

    return bt_gatt_attr_read(conn, attr, buf, len, offset, sensor_app_config->sensor_2_latest_data, sizeof(sensor_app_config->sensor_2_latest_data));
}

static ssize_t read_sensor2_data_time(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_app_config->sensor_2_latest_data_timestamp, sizeof(sensor_app_config->sensor_2_latest_data_timestamp));
}

/* LED Button Service Declaration */
BT_GATT_SERVICE_DEFINE(sensor_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_SENSOR),

    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR_STATE, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor_state, write_sensor_state, NULL),

    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_ENABLED, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_enabled, write_sensor1_enabled, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_CONFIG, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_config, write_sensor1_config, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_PWR_CONFIG, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_pwr_config, write_sensor1_pwr_config, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_DATA_FREQ, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_data_freq, write_sensor1_data_freq, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_DATA, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_sensor1_data, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_DATA_TIMESTAMP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_sensor1_data_time, NULL, NULL),

    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR2_ENABLED, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor2_enabled, write_sensor2_enabled, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR2_CONFIG, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor2_config, write_sensor2_config, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR2_PWR_CONFIG, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor2_pwr_config, write_sensor2_pwr_config, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR2_DATA_FREQ, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor2_data_freq, write_sensor2_data_freq, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR2_DATA, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_sensor2_data, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR2_DATA_TIMESTAMP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_sensor2_data_time, NULL, NULL),
);

int ble_sensor_service_init(sensor_app_config_t *config)
{
    sensor_app_config = config;
    is_sensor_service_setup = 1;
    return 0;
}