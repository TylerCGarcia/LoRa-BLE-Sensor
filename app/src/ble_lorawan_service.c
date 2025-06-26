
#include "ble_lorawan_service.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>
#include "sensor_nvs.h"
#include "sensor_app.h"

LOG_MODULE_REGISTER(BLE_LORAWAN_SERVICE, LOG_LEVEL_INF);


static lorawan_setup_t *lorawan_service_setup;
static int is_lorawan_service_setup = 0;

static ssize_t read_enabled(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &lorawan_service_setup->is_lorawan_enabled, sizeof(lorawan_service_setup->is_lorawan_enabled));
}

static ssize_t write_enabled(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(lorawan_service_setup->is_lorawan_enabled)) {
		LOG_ERR("Write date: Data length incorrect for LoRaWAN enabled");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for LoRaWAN enabled");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into lorawan enabled buffer
	memcpy(&lorawan_service_setup->is_lorawan_enabled, buf, len);

	LOG_INF("LoRaWAN enabled new value: %d", lorawan_service_setup->is_lorawan_enabled);
	sensor_nvs_write(SENSOR_NVS_ADDRESS_LORAWAN_ENABLED, &lorawan_service_setup->is_lorawan_enabled, sizeof(lorawan_service_setup->is_lorawan_enabled));
	return len;
}

static ssize_t read_frequency(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &lorawan_service_setup->lorawan_frequency, sizeof(lorawan_service_setup->lorawan_frequency));
}

static ssize_t write_frequency(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(lorawan_service_setup->lorawan_frequency)) {
		LOG_ERR("Write date: Data length incorrect for LoRaWAN frequency");
		LOG_ERR("len: %d, expected: %d", len, sizeof(lorawan_service_setup->lorawan_frequency));
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for LoRaWAN frequency");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into lorawan enabled buffer
	memcpy(&lorawan_service_setup->lorawan_frequency, buf, len);

	LOG_INF("LoRaWAN frequency new value: %d", lorawan_service_setup->lorawan_frequency);
	sensor_nvs_write(SENSOR_NVS_ADDRESS_LORAWAN_FREQUENCY, &lorawan_service_setup->lorawan_frequency, sizeof(lorawan_service_setup->lorawan_frequency));
	return len;
}

static ssize_t read_dev_eui(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, lorawan_service_setup->dev_eui, sizeof(lorawan_service_setup->dev_eui));
}

static ssize_t write_dev_eui(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(lorawan_service_setup->dev_eui)) {
		LOG_ERR("Write date: Data length incorrect for DEV EUI");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for DEV EUI");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Store incoming data into dev_eui buffer
	memcpy(lorawan_service_setup->dev_eui, buf, len);

	LOG_INF("DEV EUI RECEIVED:  0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", \
		lorawan_service_setup->dev_eui[0],lorawan_service_setup->dev_eui[1],lorawan_service_setup->dev_eui[2],lorawan_service_setup->dev_eui[3], \
		lorawan_service_setup->dev_eui[4],lorawan_service_setup->dev_eui[5],lorawan_service_setup->dev_eui[6],lorawan_service_setup->dev_eui[7]);
	sensor_nvs_write(SENSOR_NVS_ADDRESS_LORAWAN_DEV_EUI, lorawan_service_setup->dev_eui, sizeof(lorawan_service_setup->dev_eui));
	return len;
}

static ssize_t read_join_eui(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, lorawan_service_setup->join_eui, sizeof(lorawan_service_setup->join_eui));
}

static ssize_t write_join_eui(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(lorawan_service_setup->join_eui)) {
		LOG_ERR("Write date: Data length incorrect for JOIN EUI");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for JOIN EUI");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
	
	// Store incoming data into join_eui buffer
	memcpy(lorawan_service_setup->join_eui, buf, len);

	LOG_INF("JOIN EUI RECEIVED:  0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", \
		lorawan_service_setup->join_eui[0],lorawan_service_setup->join_eui[1],lorawan_service_setup->join_eui[2],lorawan_service_setup->join_eui[3], \
		lorawan_service_setup->join_eui[4],lorawan_service_setup->join_eui[5],lorawan_service_setup->join_eui[6],lorawan_service_setup->join_eui[7]);
	sensor_nvs_write(SENSOR_NVS_ADDRESS_LORAWAN_JOIN_EUI, lorawan_service_setup->join_eui, sizeof(lorawan_service_setup->join_eui));
	return len;
}

static ssize_t read_app_key(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_READ_NOT_PERMITTED);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, lorawan_service_setup->app_key, sizeof(lorawan_service_setup->app_key));
}

static ssize_t write_app_key(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if(!is_lorawan_service_setup)
	{
		LOG_ERR("LoRaWAN BLE Service is not initialized");
		return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
	}

	// Check bounds of data
	if (len != sizeof(lorawan_service_setup->app_key)) {
		LOG_ERR("Write date: Data length incorrect for APP KEY");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_ERR("Write date: Incorrect data offset for APP KEY");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
	
	// Store incoming data into app_key buffer
	memcpy(lorawan_service_setup->app_key, buf, len);

	LOG_INF("APP KEY RECEIVED:  0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", \
		lorawan_service_setup->app_key[0],lorawan_service_setup->app_key[1],lorawan_service_setup->app_key[2],lorawan_service_setup->app_key[3], \
		lorawan_service_setup->app_key[4],lorawan_service_setup->app_key[5],lorawan_service_setup->app_key[6],lorawan_service_setup->app_key[7], \
		lorawan_service_setup->app_key[8],lorawan_service_setup->app_key[9],lorawan_service_setup->app_key[10],lorawan_service_setup->app_key[11], \
		lorawan_service_setup->app_key[12],lorawan_service_setup->app_key[13],lorawan_service_setup->app_key[14],lorawan_service_setup->app_key[15]);
	sensor_nvs_write(SENSOR_NVS_ADDRESS_LORAWAN_APP_KEY, lorawan_service_setup->app_key, sizeof(lorawan_service_setup->app_key));
	return len;
}

/* LoRaWAN Service Declaration */
BT_GATT_SERVICE_DEFINE(lorawan_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_LORAWAN),

	BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_ENABLED, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_enabled, write_enabled, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_FREQUENCY, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_frequency, write_frequency, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_DEV_EUI, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_dev_eui, write_dev_eui, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_JOIN_EUI, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_join_eui, write_join_eui, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_APP_KEY, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_app_key, write_app_key, NULL),

);

int ble_lorawan_service_init(lorawan_setup_t *setup)
{
	lorawan_service_setup = setup;
	is_lorawan_service_setup = 1;
	return 0;
}