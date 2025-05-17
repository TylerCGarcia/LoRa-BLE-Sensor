
#include "sensor_ble_fakes.h"

DEFINE_FAKE_VALUE_FUNC(int, ble_setup, ble_config_t *);
DEFINE_FAKE_VALUE_FUNC(int, is_ble_advertising);
DEFINE_FAKE_VALUE_FUNC(int, ble_change_name, ble_config_t *);
DEFINE_FAKE_VALUE_FUNC(adv_interval_t, get_ble_adv_interval);

// Reset all fakes
void sensor_ble_fakes_reset(void)
{
    RESET_FAKE(ble_setup);
    RESET_FAKE(is_ble_advertising);
    RESET_FAKE(ble_change_name);
    RESET_FAKE(get_ble_adv_interval);
}