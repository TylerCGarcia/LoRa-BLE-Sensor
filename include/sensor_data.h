#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

/**
 * @brief Sensor identifiers
 */
typedef enum sensor_id {
    SENSOR_1 = 0,
    SENSOR_2,
    SENSOR_INDEX_LIMIT // This is used to get the size of the enum
};

/**
 * @brief Regulator identifiers
 */
typedef enum sensor_types {
    /* No sensor configuration is set. */
    NULL_SENSOR = 0,
    /* Sensor is set to read from voltage_read pin. */
    VOLTAGE_SENSOR,
    /* Sensor is set to read from current_read pin. */
    CURRENT_SENSOR,
    /* Sensor is set to read low pulses from d1 pin. */
    PULSE_SENSOR,
    /* Used to denote size of enum. */
    SENSOR_TYPE_LIMIT
};


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
} sensor_data_config_t;

#define VOLTAGE_READ_DIVIDER_HIGH     100
#define VOLTAGE_READ_DIVIDER_LOW      13
#define CURRENT_READ_RESISTOR         50

int sensor_data_setup(sensor_data_config_t *config, enum sensor_types sensor_type);

enum sensor_types get_sensor_data_setup(sensor_data_config_t *config);

float get_sensor_voltage_reading(sensor_data_config_t *config);

float get_sensor_current_reading(sensor_data_config_t *config);

int get_sensor_pulse_count(sensor_data_config_t *config);

int reset_sensor_pulse_count(sensor_data_config_t *config);

#endif
