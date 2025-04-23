#ifndef SENSOR_POWER_H
#define SENSOR_POWER_H

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

/**
 * @brief Regulator identifiers
 */
typedef enum sensor_output_id {
    SENSOR_OUTPUT_1 = 0,
    SENSOR_OUTPUT_2,
    SENSOR_OUTPUT_INDEX_LIMIT // This is used to get the size of the enum
};

/**
 * @brief Voltage levels for sensor power
 */
typedef enum sensor_voltage {
    SENSOR_VOLTAGE_OFF = 0,
    SENSOR_VOLTAGE_3V3,
    SENSOR_VOLTAGE_5V,
    SENSOR_VOLTAGE_6V,
    SENSOR_VOLTAGE_12V,
    SENSOR_VOLTAGE_24V,
    SENSOR_VOLTAGE_INDEX_LIMIT
};

/**
 * @brief Sensor power configuration
 */
typedef struct {
    enum sensor_output_id output_id;
    struct gpio_dt_spec boost_en; 
    struct gpio_dt_spec boost_ctrl1;
    struct gpio_dt_spec boost_ctrl2;
    const struct device *ldo_dev;
    const struct adc_dt_spec output_read;
} sensor_power_config_t;

/**
 * @brief Initialize a sensor_power_setup. This sets the id of the output, the gpios beings used and the regulator device being used.
 * After configurations are set, the sensors output is set OFF.
 * 
 * @param config 
 * @return int 
 */
int sensor_power_init(sensor_power_config_t *config);

/**
 * @brief Get the sensor voltage that is set for an output.
 * 
 * @param config 
 * @return enum sensor_voltage 
 */
enum sensor_voltage get_sensor_output(sensor_power_config_t *config);

/**
 * @brief Set the sensor voltage for an output, enabling/disabling the correct regulators, and setting the correct gpios.
 * 
 * @param config 
 * @param voltage 
 * @return int 
 */
int set_sensor_output(sensor_power_config_t *config, enum sensor_voltage voltage);


float read_sensor_output(sensor_power_config_t *config);

#endif

