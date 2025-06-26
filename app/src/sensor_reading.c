

#include "sensor_reading.h"
#include <zephyr/kernel.h>

typedef struct {
    struct gpio_callback cb;
    enum sensor_id id;
} pulse_context_t;

enum sensor_types sensor_setups[SENSOR_INDEX_LIMIT];

static int pulse_count[SENSOR_INDEX_LIMIT];

static pulse_context_t pulse_cb_data[SENSOR_INDEX_LIMIT];

static int64_t last_pulse_time[SENSOR_INDEX_LIMIT];

/* Button Interrupt */
static void pulse_captured(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    pulse_context_t *context = CONTAINER_OF(cb, pulse_context_t, cb);
    int id = context->id;
    int64_t now = k_uptime_get();

    if (now - last_pulse_time[id] > PULSE_DEBOUNCE_MS) {
        pulse_count[id]++;
        last_pulse_time[id] = now;
    }
}

static int sensor_reading_pulse_setup(sensor_reading_config_t *config)
{
    int ret;

    if (!gpio_is_ready_dt(&config->d1)) 
    {
        return -1;
    }

    ret = gpio_pin_configure_dt(&config->d1, GPIO_INPUT);
    if (ret != 0) 
    {
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&config->d1, GPIO_INT_EDGE_TO_INACTIVE);
    if (ret != 0) 
    {
        return -1;
    }

    pulse_cb_data[config->id].id = config->id;

    gpio_init_callback(&pulse_cb_data[config->id].cb, pulse_captured, BIT(config->d1.pin));
    gpio_add_callback(config->d1.port, &pulse_cb_data[config->id].cb);

    return 0;
}

static int sensor_reading_adc_setup(sensor_reading_config_t *config, enum sensor_types sensor_type)
{
    int ret;
    if(sensor_type == VOLTAGE_SENSOR)
    {
        if (!adc_is_ready_dt(&config->voltage_read)) {
            return -1;
        }
        ret = adc_channel_setup_dt(&config->voltage_read);
        if (ret < 0) {
            return ret;
        }
        return 0;
    }
    else if(sensor_type == CURRENT_SENSOR)
    {
        if (!adc_is_ready_dt(&config->current_read)) {
            return -1;
        }
        ret = adc_channel_setup_dt(&config->current_read);
        if (ret < 0) {
            return ret;
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

int sensor_reading_setup(sensor_reading_config_t *config, enum sensor_types sensor_type)
{
    int ret;
    // If switching from pulse sensor to any other sensor remove the callback
    if (sensor_setups[config->id] == PULSE_SENSOR && sensor_type != PULSE_SENSOR)
    {
        gpio_remove_callback(config->d1.port, &pulse_cb_data[config->id].cb);
        reset_sensor_pulse_count(config);
    }

    switch (sensor_type)
    {
    case NULL_SENSOR:
        
        break;
    case VOLTAGE_SENSOR:
        ret = sensor_reading_adc_setup(config, sensor_type);
        break;
    case CURRENT_SENSOR:
        ret = sensor_reading_adc_setup(config, sensor_type);
        break;
    case PULSE_SENSOR:
        ret = sensor_reading_pulse_setup(config);
        break;
    default:
        sensor_setups[config->id] = NULL_SENSOR;
        break;
    }

    if(ret < 0)
    {
        sensor_setups[config->id] = NULL_SENSOR;
        return ret;
    }
    
    sensor_setups[config->id] = sensor_type;
    return 0;
}

enum sensor_types get_sensor_reading_setup(sensor_reading_config_t *config)
{
    return sensor_setups[config->id];
}

static int read_sensor_output_raw(const struct adc_dt_spec *spec)
{
	int16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
		// Optional
		.calibrate = true,
	};
	int err = adc_sequence_init_dt(spec, &sequence);
    if(err < 0)
    {
        return err;
    }
	err = adc_read(spec->dev, &sequence);
    if(err < 0)
    {
        return err;
    }
    return buf;
}

float get_sensor_voltage_reading(sensor_reading_config_t *config)
{
    if(get_sensor_reading_setup(config) != VOLTAGE_SENSOR)
    {
        return -1;
    }
	int val_mv = (int)read_sensor_output_raw(&config->voltage_read);
	int err = adc_raw_to_millivolts_dt(&config->voltage_read, &val_mv);
    if(err < 0)
    {
        return err;
    }
	return (((float)val_mv/1000.0f) * (((float)VOLTAGE_READ_DIVIDER_HIGH + (float)VOLTAGE_READ_DIVIDER_LOW)/(float)VOLTAGE_READ_DIVIDER_LOW));
}

float get_sensor_current_reading(sensor_reading_config_t *config)
{
    if(get_sensor_reading_setup(config) != CURRENT_SENSOR)
    {
        return -1;
    }
	int val_mv = (int)read_sensor_output_raw(&config->current_read);
	int err = adc_raw_to_millivolts_dt(&config->current_read, &val_mv);
    if(err < 0)
    {
        return err;
    }
    // I = V/R
    return (float)val_mv / (float)CURRENT_READ_RESISTOR;
}

int get_sensor_pulse_count(sensor_reading_config_t *config)
{
    if(get_sensor_reading_setup(config) != PULSE_SENSOR)
    {
        return -1;
    }
    return pulse_count[config->id];
}

int reset_sensor_pulse_count(sensor_reading_config_t *config)
{
    if(get_sensor_reading_setup(config) != PULSE_SENSOR)
    {
        return -1;
    }
    pulse_count[config->id] = 0;
    return 0;
}