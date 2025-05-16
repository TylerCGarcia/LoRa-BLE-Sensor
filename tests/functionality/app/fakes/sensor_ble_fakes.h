#ifndef SENSOR_BLE_FAKES_H
#define SENSOR_BLE_FAKES_H

#include <zephyr/fff.h>
#include <stdint.h>
#include <zephyr/bluetooth/bluetooth.h>

/* Structure to hold the BLE setup. */
typedef struct {
    /* Advertising options. */
    enum bt_le_adv_opt adv_opt;
    /* Advertising interval min in milliseconds. */
    uint32_t adv_interval_min_ms;
    /* Advertising interval max in milliseconds. */
    uint32_t adv_interval_max_ms;
    /* Connection callbacks. */
    struct bt_conn_cb *connection_callbacks;
    /* If to add service uuid to the advertising data, 1 to add, 0 to not add. */
    uint8_t add_service_uuid;
    /* Advertising name. */
    char adv_name[32];
    /* Advertising data. */
    uint8_t *adv_data;
    /* Advertising data length. */
    uint8_t adv_data_len;
} ble_config_t;

/* Structure to hold the advertising intervals using zephyr codes(not milliseconds). */
typedef struct {
    /* Advertising interval min in zephyr codes(not milliseconds). */
    uint32_t min;
    /* Advertising interval max in zephyr codes(not milliseconds). */
    uint32_t max;
} adv_interval_t;

DECLARE_FAKE_VALUE_FUNC(int, ble_setup, ble_config_t *);
DECLARE_FAKE_VALUE_FUNC(int, is_ble_advertising);
DECLARE_FAKE_VALUE_FUNC(int, ble_change_name, ble_config_t *);
DECLARE_FAKE_VALUE_FUNC(adv_interval_t, get_ble_adv_interval);

#endif /* SENSOR_BLE_FAKES_H */ 