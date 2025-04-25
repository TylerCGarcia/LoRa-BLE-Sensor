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

#define VOLTAGE_NAME_LENGTH         20
#define OUTUT_READ_DIVIDER_HIGH     100
#define OUTUT_READ_DIVIDER_LOW      13


typedef struct {
    enum sensor_voltage voltage_enum;
    const char *name;
    float expected_output;
} sensor_voltage_info_t;

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
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @return int -1 if error setting up power system or adc
 */
int sensor_power_init(sensor_power_config_t *config);

/**
 * @brief Get the sensor voltage that is set for an output.
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @return enum sensor_voltage for selected sensor configuration
 */
enum sensor_voltage get_sensor_output(sensor_power_config_t *config);

/**
 * @brief Set the sensor voltage for an output, enabling/disabling the correct regulators, and setting the correct gpios.
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @param voltage enum sensor_voltage setting selected
 * @return int 0 if successful, sets voltage to OFF if invalid voltage input
 */
int set_sensor_output(sensor_power_config_t *config, enum sensor_voltage voltage);

/**
 * @brief Read the voltage output of the selected sensor power configuration. Takes into account resistor divider on output.
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @return float output of the sensor power system
 */
float read_sensor_output(sensor_power_config_t *config);

/**
 * @brief 
 * 
 * @param config sensor_power_config_t sensor power configuration for the current sensor
 * @param voltage enum sensor_voltage setting selected
 * @param accepted_error error in percent that the device is allowed to be off by
 * @return int 0 if successful, -1 if not
 */
int validate_output(sensor_power_config_t *config, enum sensor_voltage voltage, uint8_t accepted_error);

/**
 * @brief Get the name for the selected sensor voltage
 * 
 * @param voltage_name char array for the name to be saved to
 * @param voltage enum sensor_voltage setting selected
 * @return int 0 if successful, -1 if invalid input
 */
int get_sensor_voltage_name(char * voltage_name, enum sensor_voltage voltage);

#endif

