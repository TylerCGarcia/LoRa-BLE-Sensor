
#include "ble_sensor_service.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(BLE_SENSOR_SERVICE, LOG_LEVEL_DBG);

static sensor_app_config_t *sensor_app_config;
static int is_sensor_service_setup = 0;

static ssize_t read_sensor1_pwr_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}
    uint8_t sensor_1_voltage = sensor_app_config->sensor_1_voltage;
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_1_voltage, sizeof(sensor_1_voltage));
}

static ssize_t write_sensor1_pwr_config(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    uint8_t sensor_1_voltage = 0;
    LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);
    if(!is_sensor_service_setup)
	{
		LOG_ERR("Sensor BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

    // Check bounds of data
	if (len != sizeof(sensor_1_voltage)) {
		LOG_ERR("Write date: Data length incorrect for DEV EUI");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for DEV EUI");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into dev_eui buffer
	memcpy(&sensor_1_voltage, buf, len);

	LOG_INF("Sensor 1 Voltage RECEIVED: %d", sensor_1_voltage);
    sensor_app_config->sensor_1_voltage = sensor_1_voltage;
	return len;
}

/* LED Button Service Declaration */
BT_GATT_SERVICE_DEFINE(sensor_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_SENSOR),
    
	BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_PWR_CONFIG, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_pwr_config, write_sensor1_pwr_config, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_CONFIG, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_config, write_sensor1_config, NULL),
    // BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_DATA, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_data, write_sensor1_data, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_DATA_TIME, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_data_time, write_sensor1_data_time, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR1_DATA_FREQ, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_sensor1_data_freq, write_sensor1_data_freq, NULL),

);

int ble_sensor_service_init(sensor_app_config_t *config)
{
    sensor_app_config = config;
    return 0;
}