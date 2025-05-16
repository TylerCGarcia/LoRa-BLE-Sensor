#include "sensor_reading_fakes.h"

// Define all the fake functions
DEFINE_FAKE_VALUE_FUNC(int, sensor_reading_setup, sensor_reading_config_t *, enum sensor_types);
DEFINE_FAKE_VALUE_FUNC(enum sensor_types, get_sensor_reading_setup, sensor_reading_config_t *);
DEFINE_FAKE_VALUE_FUNC(float, get_sensor_voltage_reading, sensor_reading_config_t *);
DEFINE_FAKE_VALUE_FUNC(float, get_sensor_current_reading, sensor_reading_config_t *);
DEFINE_FAKE_VALUE_FUNC(int, get_sensor_pulse_count, sensor_reading_config_t *);
DEFINE_FAKE_VALUE_FUNC(int, reset_sensor_pulse_count, sensor_reading_config_t *);