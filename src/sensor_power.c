#include "sensor_power.h"
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/adc.h>

static int output_read_divider_high = 100;
static int output_read_divider_low = 13;

const sensor_voltage_info_t sensor_voltage_table[] = {
    { SENSOR_VOLTAGE_OFF, "SENSOR_VOLTAGE_OFF", 2.75 }, // <- substitute real value for OFF if needed
    { SENSOR_VOLTAGE_3V3, "SENSOR_VOLTAGE_3V3", 3.3 },
    { SENSOR_VOLTAGE_5V,  "SENSOR_VOLTAGE_5V",  5.0 },
    { SENSOR_VOLTAGE_6V,  "SENSOR_VOLTAGE_6V",  6.0 },
    { SENSOR_VOLTAGE_12V, "SENSOR_VOLTAGE_12V", 12.0 },
    { SENSOR_VOLTAGE_24V, "SENSOR_VOLTAGE_24V", 24.0 },
};

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

int set_sensor_output(sensor_power_config_t *config, enum sensor_voltage voltage)
{
    sensor_state[config->output_id] = voltage;
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
            set_sensor_output(config, SENSOR_VOLTAGE_OFF); // If invalid output, set voltage to OFF
            return -EINVAL; 
    }
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

static int sensor_output_adc_setup(sensor_power_config_t *config)
{
    int ret;
    // if (!device_is_ready(config->output_read.dev)) {
    //     return -1;
	// }	
    // struct adc_channel_cfg channel_cfg = {
    // .gain             = ADC_GAIN_1_6,
    // .reference        = ADC_REF_INTERNAL,
    // .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    // .channel_id       = &config->output_read.channel_id,
    // };
    // ret = adc_channel_setup(config->output_read.dev, &channel_cfg);
    // if (ret < 0) {
	// 	return ret;
    // }
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
    sensor_power_setup(config);
    sensor_output_adc_setup(config);
    set_sensor_output(config, SENSOR_VOLTAGE_OFF);
    return 0;
}

enum sensor_voltage get_sensor_output(sensor_power_config_t *config)
{
    return sensor_state[config->output_id];
}

static int read_sensor_output_raw(sensor_power_config_t *config)
{
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

	int val_mv = (int)read_sensor_output_raw(config);
    // val_mv = (val_mv*600*6) / 4095; // Unused custom calculations
	int err = adc_raw_to_millivolts_dt(&config->output_read, &val_mv);
    if(err < 0)
    {
        return err;
    }
	return (((float)val_mv/1000.0) * (((float)output_read_divider_high + (float)output_read_divider_low)/(float)output_read_divider_low));
}

int validate_output(sensor_power_config_t *config, enum sensor_voltage voltage, uint8_t accepted_error)
{
    if(voltage != get_sensor_output(config))
    {
        return -1;
    }
    return 0;
}