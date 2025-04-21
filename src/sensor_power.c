#include "sensor_power.h"
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/regulator.h>


enum sensor_voltage sensor_state[SENSOR_OUTPUT_INDEX_LIMIT];

static int turn_off_regulator(sensor_power_config_t *config)
{
    if(regulator_is_enabled(config->ldo_dev))
    {
        regulator_disable(config->ldo_dev);
    }
}
static int turn_on_regulator(sensor_power_config_t *config)
{
    if(!regulator_is_enabled(config->ldo_dev))
    {
        regulator_enable(config->ldo_dev);
    }
}

int set_sensor_voltage(sensor_power_config_t *config, enum sensor_voltage voltage)
{
    switch (voltage) {
        case SENSOR_VOLTAGE_OFF:
            turn_off_regulator(config);
            gpio_pin_set_dt(&config->boost_en, 0);
            gpio_pin_set_dt(&config->boost_ctrl1, 0);
            gpio_pin_set_dt(&config->boost_ctrl2, 0);
            break;
        case SENSOR_VOLTAGE_3V3:
            turn_on_regulator(config);
            gpio_pin_set_dt(&config->boost_en, 0);
            gpio_pin_set_dt(&config->boost_ctrl1, 0);
            gpio_pin_set_dt(&config->boost_ctrl2, 0);
            break;
        case SENSOR_VOLTAGE_5V:
            turn_off_regulator(config);
            gpio_pin_set_dt(&config->boost_en, 1);
            gpio_pin_set_dt(&config->boost_ctrl1, 1);
            gpio_pin_set_dt(&config->boost_ctrl2, 1);
            break;
        case SENSOR_VOLTAGE_6V:
            turn_off_regulator(config);
            gpio_pin_set_dt(&config->boost_en, 1);
            gpio_pin_set_dt(&config->boost_ctrl1, 0);
            gpio_pin_set_dt(&config->boost_ctrl2, 1);
            break;  
        case SENSOR_VOLTAGE_12V:
            turn_off_regulator(config);
            gpio_pin_set_dt(&config->boost_en, 1);
            gpio_pin_set_dt(&config->boost_ctrl1, 1);
            gpio_pin_set_dt(&config->boost_ctrl2, 0);
            break;
        case SENSOR_VOLTAGE_24V:
            turn_off_regulator(config);
            gpio_pin_set_dt(&config->boost_en, 1);
            gpio_pin_set_dt(&config->boost_ctrl1, 0);
            gpio_pin_set_dt(&config->boost_ctrl2, 0);
            break;
        default:
            set_sensor_voltage(config, SENSOR_VOLTAGE_OFF); // If invalid output, set voltage to OFF
            return -EINVAL; 
    }
    sensor_state[config->output_id] = voltage;
    return 0;
    }

static int sensor_power_setup(sensor_power_config_t *config)
{
    int ret;
	if (!gpio_is_ready_dt(&config->boost_en)) {
		return -1;
	}
	if (!gpio_is_ready_dt(&config->boost_ctrl1)) {
		return -1;
	}
	if (!gpio_is_ready_dt(&config->boost_ctrl2)) {
		return -1;
	}
	ret = gpio_pin_configure_dt(&config->boost_en, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return ret;
	}
	ret = gpio_pin_configure_dt(&config->boost_ctrl1, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return ret;
	}
	ret = gpio_pin_configure_dt(&config->boost_ctrl2, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return ret;
	}
    if (!device_is_ready(config->ldo_dev)) {
        return -1;
	}	
    /* Now set the final desired voltage */
    ret = regulator_set_voltage(config->ldo_dev, 3300000, 3300000);
    if (ret < 0) {
		return ret;
    }
    return 0;
}

int sensor_power_init(sensor_power_config_t *config)
{
    sensor_power_setup(config);
    set_sensor_voltage(config, SENSOR_VOLTAGE_OFF);
    return 0;
}

enum sensor_voltage get_sensor_voltage(sensor_power_config_t *config)
{
    return sensor_state[config->output_id];
}

