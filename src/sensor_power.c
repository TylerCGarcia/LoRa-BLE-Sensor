#include "sensor_power.h"
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/adc.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor_power, LOG_LEVEL_DBG);

const sensor_voltage_info_t sensor_voltage_table[] = {
    { SENSOR_VOLTAGE_OFF, "SENSOR_VOLTAGE_OFF", 2.75 },
    { SENSOR_VOLTAGE_3V3, "SENSOR_VOLTAGE_3V3", 3.3 },
    { SENSOR_VOLTAGE_5V,  "SENSOR_VOLTAGE_5V",  5.0 },
    { SENSOR_VOLTAGE_6V,  "SENSOR_VOLTAGE_6V",  6.0 },
    { SENSOR_VOLTAGE_12V, "SENSOR_VOLTAGE_12V", 12.0 },
    { SENSOR_VOLTAGE_24V, "SENSOR_VOLTAGE_24V", 24.0 },
};

enum sensor_voltage sensor_state[SENSOR_POWER_INDEX_LIMIT];

static int turn_off_regulator(sensor_power_config_t *config)
{
    if(regulator_is_enabled(config->ldo_dev))
    {
        LOG_DBG("Disabling regulator");
        regulator_disable(config->ldo_dev);
    }
}
static int turn_on_regulator(sensor_power_config_t *config)
{
    if(!regulator_is_enabled(config->ldo_dev))
    {
        LOG_DBG("Enabling regulator");
        regulator_enable(config->ldo_dev);
    }
}

int set_sensor_output(sensor_power_config_t *config, enum sensor_voltage voltage)
{
    // If the voltage is being set to a non-off voltage, and the current voltage is not off, set the voltage to off first.
    if(voltage != SENSOR_VOLTAGE_OFF && sensor_state[config->power_id] != SENSOR_VOLTAGE_OFF)
    {
        set_sensor_output(config, SENSOR_VOLTAGE_OFF);
    }
    sensor_state[config->power_id] = voltage;
    switch (voltage) {
    case SENSOR_VOLTAGE_OFF:
        LOG_DBG("Setting voltage to OFF");
        turn_off_regulator(config);
        gpio_pin_set_dt(&config->boost_en, 0);
        gpio_pin_set_dt(&config->boost_ctrl1, 0);
        gpio_pin_set_dt(&config->boost_ctrl2, 0);
        break;
    case SENSOR_VOLTAGE_3V3:
        LOG_DBG("Setting voltage to 3.3V");
        turn_on_regulator(config);
        gpio_pin_set_dt(&config->boost_en, 0);
        gpio_pin_set_dt(&config->boost_ctrl1, 0);
        gpio_pin_set_dt(&config->boost_ctrl2, 0);
        break;
    case SENSOR_VOLTAGE_5V:
        LOG_DBG("Setting voltage to 5V");
        turn_off_regulator(config);
        gpio_pin_set_dt(&config->boost_en, 1);
        gpio_pin_set_dt(&config->boost_ctrl1, 1);
        gpio_pin_set_dt(&config->boost_ctrl2, 1);
        break;
    case SENSOR_VOLTAGE_6V:
        LOG_DBG("Setting voltage to 6V");
        turn_off_regulator(config);
        gpio_pin_set_dt(&config->boost_en, 1);
        gpio_pin_set_dt(&config->boost_ctrl1, 0);
        gpio_pin_set_dt(&config->boost_ctrl2, 1);
        break;  
    case SENSOR_VOLTAGE_12V:
        LOG_DBG("Setting voltage to 12V");
        turn_off_regulator(config);
        gpio_pin_set_dt(&config->boost_en, 1);
        gpio_pin_set_dt(&config->boost_ctrl1, 1);
        gpio_pin_set_dt(&config->boost_ctrl2, 0);
        break;
    case SENSOR_VOLTAGE_24V:
        LOG_DBG("Setting voltage to 24V");
        turn_off_regulator(config);
        gpio_pin_set_dt(&config->boost_en, 1);
        gpio_pin_set_dt(&config->boost_ctrl1, 0);
        gpio_pin_set_dt(&config->boost_ctrl2, 0);
        break;
    default:
        set_sensor_output(config, SENSOR_VOLTAGE_OFF); // If invalid output, set voltage to OFF
        return -EINVAL; 
    }
    k_msleep(config->delay_ms);
    return 0;
}

static int sensor_power_setup(sensor_power_config_t *config)
{
    int ret;
	if (!gpio_is_ready_dt(&config->boost_en)) 
    {
		return -1;
	}
	if (!gpio_is_ready_dt(&config->boost_ctrl1)) 
    {
		return -1;
	}
	if (!gpio_is_ready_dt(&config->boost_ctrl2)) 
    {
		return -1;
	}
	ret = gpio_pin_configure_dt(&config->boost_en, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) 
    {
		return ret;
	}
	ret = gpio_pin_configure_dt(&config->boost_ctrl1, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) 
    {
		return ret;
	}
	ret = gpio_pin_configure_dt(&config->boost_ctrl2, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) 
    {
		return ret;
	}
    if (!device_is_ready(config->ldo_dev)) 
    {
        return -1;
	}	
    /* Now set the final desired voltage */
    ret = regulator_set_voltage(config->ldo_dev, 3300000, 3300000);
    if (ret < 0) 
    {
		return ret;
    }

    return 0;
}

static int sensor_output_adc_setup(sensor_power_config_t *config)
{
    int ret;
    LOG_DBG("Setting up ADC for sensor output");
    if (!adc_is_ready_dt(&config->output_read)) {
		return -1;
	}
    ret = adc_channel_setup_dt(&config->output_read);
    if (ret < 0) {
		return ret;
    }
    return 0;
}

int sensor_power_init(sensor_power_config_t *config)
{
    LOG_DBG("Initializing sensor power");
    if(sensor_power_setup(config) != 0)
    {
        return -1;
    }
    if(sensor_output_adc_setup(config) != 0)
    {
        return -1;
    }
    set_sensor_output(config, SENSOR_VOLTAGE_OFF);
    return 0;
}

enum sensor_voltage get_sensor_output(sensor_power_config_t *config)
{
    LOG_DBG("Getting sensor output");
    return sensor_state[config->power_id];
}

static int read_sensor_output_raw(sensor_power_config_t *config)
{
    LOG_DBG("Reading sensor output raw");
	int16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
		// Optional
		.calibrate = true,
	};
	int err = adc_sequence_init_dt(&config->output_read, &sequence);
    if(err < 0)
    {
        return err;
    }
	err = adc_read(config->output_read.dev, &sequence);
    if(err < 0)
    {
        return err;
    }
    return buf;
}

float read_sensor_output(sensor_power_config_t *config)
{
    LOG_DBG("Reading sensor output");
	int val_mv = (int)read_sensor_output_raw(config);
    // val_mv = (val_mv*600*6) / 4095; // Unused custom calculations
	int err = adc_raw_to_millivolts_dt(&config->output_read, &val_mv);
    if(err < 0)
    {
        return err;
    }
	return (((float)val_mv/1000.0) * (((float)OUTUT_READ_DIVIDER_HIGH + (float)OUTUT_READ_DIVIDER_LOW)/(float)OUTUT_READ_DIVIDER_LOW));
}

int validate_output(sensor_power_config_t *config, enum sensor_voltage voltage, uint8_t accepted_error)
{
    LOG_DBG("Validating output");
    if(voltage != get_sensor_output(config))
    {
        return -1;
    }
    float sensor_reading = read_sensor_output(config);
    float upper_bounds = sensor_voltage_table[voltage].expected_output * (float)((100.0 + (float)accepted_error)/100.0);
    float lower_bounds = sensor_voltage_table[voltage].expected_output * (float)((100.0 - (float)accepted_error)/100.0);
    if(sensor_reading < lower_bounds || sensor_reading > upper_bounds)
    {
        LOG_ERR("Sensor output out of bounds, expected %f, got %f", sensor_voltage_table[voltage].expected_output, sensor_reading);
        return -1;
    }
    return 0;
}

int get_sensor_voltage_name(char * voltage_name, enum sensor_voltage voltage)
{
    LOG_DBG("Getting sensor voltage name");
    if(voltage < 0 || voltage >= SENSOR_VOLTAGE_INDEX_LIMIT)
    {
        LOG_ERR("Invalid voltage");
        return -1;
    }
    strcpy(voltage_name, sensor_voltage_table[voltage].name);
    return 0;
}

