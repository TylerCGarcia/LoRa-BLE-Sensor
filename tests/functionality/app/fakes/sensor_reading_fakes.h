#ifndef SENSOR_READING_FAKES_H
#define SENSOR_READING_FAKES_H

#include "sensor_id.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/fff.h>

/**
 * @brief Sensor data pin configuration
 */
typedef struct {
    /* Identification number of sensor. */
    enum sensor_id id;
    /* First Digital Pin. */
    struct gpio_dt_spec d1;
    /* Second Digital Pin. */ 
    struct gpio_dt_spec d2;
    /* ADC spec for voltage sensor. */
    const struct adc_dt_spec voltage_read;
    /* ADC spec for current sensor. */
    const struct adc_dt_spec current_read;
} sensor_reading_config_t;

// Declare all the fake functions
DECLARE_FAKE_VALUE_FUNC(int, sensor_reading_setup, sensor_reading_config_t *, enum sensor_types);
DECLARE_FAKE_VALUE_FUNC(enum sensor_types, get_sensor_reading_setup, sensor_reading_config_t *);
DECLARE_FAKE_VALUE_FUNC(float, get_sensor_voltage_reading, sensor_reading_config_t *);
DECLARE_FAKE_VALUE_FUNC(float, get_sensor_current_reading, sensor_reading_config_t *);
DECLARE_FAKE_VALUE_FUNC(int, get_sensor_pulse_count, sensor_reading_config_t *);
DECLARE_FAKE_VALUE_FUNC(int, reset_sensor_pulse_count, sensor_reading_config_t *);

// Reset all fakes
void sensor_reading_fakes_reset(void);

#endif