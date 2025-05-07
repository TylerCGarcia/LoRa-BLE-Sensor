
#include "ble_lorawan_service.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>



/* STEP 5 - Implement the read callback function of the Button characteristic */
static ssize_t read_dev_eui(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	// get a pointer to button_state which is passed in the BT_GATT_CHARACTERISTIC() and stored in attr->user_data
	// const char *value = attr->user_data;

	// if (lorawan_cb.dev_eui_cb) {
	// 	// Call the application callback function to update the get the current value of the button
	// 	dev_eui = lorawan_cb.dev_eui_cb();
	// 	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
	// }

	return 0;
}


/* LED Button Service Declaration */
/* STEP 2 - Create and add the MY LBS service to the Bluetooth LE stack */
BT_GATT_SERVICE_DEFINE(lorawan_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_LORAWAN),
    
	BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_DEV_EUI, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_dev_eui, NULL, &dev_eui),
    BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_APP_EUI, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_app_eui, NULL, &app_eui),
    BT_GATT_CHARACTERISTIC(BT_UUID_LORAWAN_NET_KEY, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_net_key, NULL, &net_key),

);