
#include "sensor_ble_fakes.h"

DEFINE_FAKE_VALUE_FUNC(int, ble_setup, ble_config_t *);
DEFINE_FAKE_VALUE_FUNC(int, is_ble_advertising);
DEFINE_FAKE_VALUE_FUNC(int, ble_change_name, ble_config_t *);
DEFINE_FAKE_VALUE_FUNC(adv_interval_t, get_ble_adv_interval);