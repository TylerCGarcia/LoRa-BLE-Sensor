// In tests/fakes/sensor_lorawan_fakes.c
#include "sensor_lorawan_fakes.h"

// Define fake functions
DEFINE_FAKE_VALUE_FUNC(int, is_lorawan_configured, lorawan_setup_t*);
DEFINE_FAKE_VOID_FUNC(sensor_lorawan_log_network_config, lorawan_setup_t*);
DEFINE_FAKE_VALUE_FUNC(int, sensor_lorawan_setup, lorawan_setup_t*);
DEFINE_FAKE_VALUE_FUNC(int, sensor_lorawan_send_data, lorawan_data_t*);
DEFINE_FAKE_VALUE_FUNC(int, is_lorawan_connected);

// Reset all fakes
void sensor_lorawan_fakes_reset(void)
{
    RESET_FAKE(is_lorawan_configured);
    RESET_FAKE(sensor_lorawan_log_network_config);
    RESET_FAKE(sensor_lorawan_setup);
    RESET_FAKE(sensor_lorawan_send_data);
    RESET_FAKE(is_lorawan_connected);
}