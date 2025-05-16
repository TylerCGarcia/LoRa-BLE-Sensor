#ifndef SENSOR_POWER_FAKES_H
#define SENSOR_POWER_FAKES_H

#include "sensor_id.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/fff.h>

/**
 * @brief Recreate the sensor_power_config_t struct for testing purposes
 */
typedef struct {
    /* Sensor power configurationid. */
    enum sensor_power_id power_id;
    /* GPIO spec for the boost enable pin. */
    struct gpio_dt_spec boost_en; 
    /* GPIO spec for the boost control 1 pin. */
    struct gpio_dt_spec boost_ctrl1;
    /* GPIO spec for the boost control 2 pin. */
    struct gpio_dt_spec boost_ctrl2;
    /* LDO device for the sensor. */
    const struct device *ldo_dev;
    /* ADC spec for reading the output voltage. */
    const struct adc_dt_spec output_read;
    /* Delay in ms to wait after setting the voltage to allow the voltage to stabilize. */
    uint32_t delay_ms;
} sensor_power_config_t;

DECLARE_FAKE_VALUE_FUNC(int, sensor_power_init, sensor_power_config_t *);
DECLARE_FAKE_VALUE_FUNC(enum sensor_voltage, get_sensor_output, sensor_power_config_t *);
DECLARE_FAKE_VALUE_FUNC(int, set_sensor_output, sensor_power_config_t *, enum sensor_voltage);
DECLARE_FAKE_VALUE_FUNC(float, read_sensor_output, sensor_power_config_t *);
DECLARE_FAKE_VALUE_FUNC(int, validate_output, sensor_power_config_t *, enum sensor_voltage, uint8_t);
DECLARE_FAKE_VALUE_FUNC(int, get_sensor_voltage_name, char *, enum sensor_voltage);

#endif