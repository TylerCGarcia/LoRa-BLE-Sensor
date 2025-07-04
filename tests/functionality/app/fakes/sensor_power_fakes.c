
#include "sensor_power_fakes.h"


DEFINE_FAKE_VALUE_FUNC(int, sensor_power_init, sensor_power_config_t *);
DEFINE_FAKE_VALUE_FUNC(enum sensor_voltage, get_sensor_output, sensor_power_config_t *);
DEFINE_FAKE_VALUE_FUNC(int, set_sensor_output, sensor_power_config_t *, enum sensor_voltage);
DEFINE_FAKE_VALUE_FUNC(float, read_sensor_output, sensor_power_config_t *);
DEFINE_FAKE_VALUE_FUNC(int, validate_output, sensor_power_config_t *, enum sensor_voltage, uint8_t);
DEFINE_FAKE_VALUE_FUNC(int, get_sensor_voltage_name, char *, enum sensor_voltage);

// Reset all fakes
void sensor_power_fakes_reset(void)
{
    RESET_FAKE(sensor_power_init);
    RESET_FAKE(get_sensor_output);
    RESET_FAKE(set_sensor_output);
    RESET_FAKE(read_sensor_output);
    RESET_FAKE(validate_output);
    RESET_FAKE(get_sensor_voltage_name);
}