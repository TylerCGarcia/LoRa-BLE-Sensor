#include "ble_device_service.h"
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

LOG_MODULE_REGISTER(BLE_DEVICE_SERVICE, LOG_LEVEL_INF);

static pmic_sensor_status_t *device_status;
static int is_device_service_setup = 0;

static ssize_t read_hardware_model(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, CONFIG_BOARD, sizeof(CONFIG_BOARD));
}

static ssize_t read_hardware_version(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, CONFIG_BOARD_REVISION, sizeof(CONFIG_BOARD_REVISION));
}

static ssize_t read_firmware_version(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, CONFIG_APP_VERSION, sizeof(CONFIG_APP_VERSION));
}

/* LoRaWAN Service Declaration */
BT_GATT_SERVICE_DEFINE(device_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_DEVICE),

	// BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_NAME, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_name, write_name, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_HARDWARE_MODEL, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_hardware_model, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_HARDWARE_VERSION, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_hardware_version, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_FIRMWARE_VERSION, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_firmware_version, NULL, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_BATTERY_PERCENT, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_battery_percent, NULL, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_BATTERY_VOLTAGE, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_battery_voltage, NULL, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_DEVICE_BATTERY_CURRENT, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_battery_current, NULL, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_FREQUENCY, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_frequency, write_frequency, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_DEV_EUI, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_dev_eui, write_dev_eui, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_JOIN_EUI, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_join_eui, write_join_eui, NULL),
	// BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_APP_KEY, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_app_key, write_app_key, NULL),

);

int ble_device_service_init(pmic_sensor_status_t *status)
{
    device_status = status;
    is_device_service_setup = 1;
    return 0;
}