#ifndef SENSOR_POWER_H
#define SENSOR_POWER_H

#include <zephyr/drivers/gpio.h>

/**
 * @brief Regulator identifiers
 */
typedef enum sensor_output_id {
    SENSOR_OUTPUT_1 = 0,
    SENSOR_OUTPUT_2,
    SENSOR_OUTPUT_MAX // This is used to get the size of the enum
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
    SENSOR_VOLTAGE_24V
};

/**
 * @brief Sensor power configuration
 */
// typedef struct {
//     enum sensor_output_id output_id;
//     const struct gpio_dt_spec *boost_en;
//     const struct gpio_dt_spec *boost_ctrl1;
//     const struct gpio_dt_spec *boost_ctrl2;
//     const struct device *ldo_dev;
// } sensor_power_config_t;
typedef struct {
    enum sensor_output_id output_id;
    struct gpio_dt_spec boost_en; 
    struct gpio_dt_spec boost_ctrl1;
    struct gpio_dt_spec boost_ctrl2;
    const struct device *ldo_dev;
} sensor_power_config_t;

int sensor_power_init(sensor_power_config_t *config);

enum sensor_voltage get_sensor_voltage(sensor_power_config_t *config);

int set_sensor_voltage(sensor_power_config_t *config, enum sensor_voltage voltage);

#endif

