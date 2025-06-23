#include "ble_device_service.h"
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

LOG_MODULE_REGISTER(BLE_DEVICE_SERVICE, LOG_LEVEL_INF);

static pmic_sensor_status_t *device_status;
static int is_device_service_setup = 0;

static ssize_t read_hardware_version(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	char hardware_version[30];
	snprintk(hardware_version, sizeof(hardware_version), "%s@%s", CONFIG_BOARD, CONFIG_BOARD_REVISION);
	LOG_INF("Hardware Version: %s", hardware_version);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, hardware_version, strlen(hardware_version));
}

static ssize_t read_firmware_version(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, CONFIG_APP_VERSION, sizeof(CONFIG_APP_VERSION));
}

static ssize_t read_battery_voltage(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	LOG_INF("Battery Voltage: %f", device_status->voltage);
	// Convert float voltage to hundredths of a volt (e.g., 3.75V becomes 375)
	int16_t voltage_hundreths = (int16_t)(device_status->voltage * 100.0f);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &voltage_hundreths, sizeof(voltage_hundreths));
}

static ssize_t read_battery_temperature(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	LOG_INF("Battery Temperature: %f", device_status->temp);
	// Convert float temperature to hundredths of a degree (e.g., 25.61°C becomes 2561)
	int16_t temp_hundredths = (int16_t)(device_status->temp * 100.0f);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &temp_hundredths, sizeof(temp_hundredths));
}

/* LoRaWAN Service Declaration */
BT_GATT_SERVICE_DEFINE(device_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_DEVICE),

	// BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_NAME, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_name, write_name, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_HARDWARE_VERSION, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_hardware_version, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_FIRMWARE_VERSION, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_firmware_version, NULL, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_BATTERY_PERCENT, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_battery_percent, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_BATTERY_VOLTAGE, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_battery_voltage, NULL, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_BATTERY_CURRENT, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_battery_current, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_BATTERY_TEMPERATURE, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_battery_temperature, NULL, NULL),
);

int ble_device_service_init(pmic_sensor_status_t *status)
{
    device_status = status;
    is_device_service_setup = 1;
    return 0;
}