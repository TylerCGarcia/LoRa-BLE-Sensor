
#include "ble_sensor_service.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

/* LED Button Service Declaration */
/* STEP 2 - Create and add the MY LBS service to the Bluetooth LE stack */
// BT_GATT_SERVICE_DEFINE(sensor_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_SENSOR),
    
// 		    //    BT_GATT_CHARACTERISTIC(BT_UUID_LBS_BUTTON, BT_GATT_CHRC_READ,
// 			// 		      BT_GATT_PERM_READ, read_button, NULL, &button_state),
// 		    //    /* STEP 4 - Create and add the LED characteristic. */
// 		    //    BT_GATT_CHARACTERISTIC(BT_UUID_LBS_LED, BT_GATT_CHRC_WRITE,
// 			// 		      BT_GATT_PERM_WRITE, NULL, write_led, NULL),

// );