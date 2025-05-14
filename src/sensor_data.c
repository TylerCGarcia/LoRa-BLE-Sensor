
#include "sensor_data.h"
#include "sensor_power.h"
#include <stdint.h>


// typedef struct data_type_info{
//     /* Type of data in the buffer. */
//     enum sensor_data_type data_type;
//     /* Size of the data in the buffer. */
//     size_t data_size;
// } data_type_info_t;

typedef struct {
    /* Power id to use for the sensor. */
    enum sensor_types type;
    /* Voltage enum to use for the sensor. */
    enum sensor_voltage voltage_enum;
    /* 1 if the sensor power is initialized, 0 if not. */
    uint8_t is_sensor_power_initialized;
    /* 1 if the power to sensor should always be on. 0 if only turned on when data is being read. */
    uint8_t is_sensor_power_continuous;
    // /* Type of data in the buffer. */
    // enum sensor_data_type data_type;
} sensor_data_config_t;

/* Sensor data configurations. */
static sensor_data_config_t sensor_data_config[SENSOR_INDEX_LIMIT];

/* Sensor power configurations for power 1. */
static sensor_power_config_t sensor_output1 = {
	.power_id = SENSOR_POWER_1,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost1en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost1ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo1)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output1),
	.delay_ms = 500
};

/* Sensor power configurations for power 2. */
static sensor_power_config_t sensor_output2 = {
	.power_id = SENSOR_POWER_2,
	.boost_en = GPIO_DT_SPEC_GET(DT_ALIAS(boost2en), gpios),	
	.boost_ctrl1 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl1), gpios),
	.boost_ctrl2 = GPIO_DT_SPEC_GET(DT_ALIAS(boost2ctrl2), gpios),
	.ldo_dev = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_ldo2)),
	.output_read = ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), sensor_output2),
	.delay_ms = 500
};

/* Array of sensor power configurations. */
static sensor_power_config_t *sensor_power_configs[] = {
    &sensor_output1,
    &sensor_output2
};

int sensor_data_setup(sensor_data_t *sensor_data, enum sensor_types type, enum sensor_voltage voltage_enum)
{
    if (sensor_data->id >= SENSOR_INDEX_LIMIT || sensor_data->power_id >= SENSOR_POWER_INDEX_LIMIT
        || type >= SENSOR_TYPE_LIMIT || voltage_enum >= SENSOR_VOLTAGE_INDEX_LIMIT) {
        return -1;
    }
    sensor_power_init(&sensor_power_configs[sensor_data->power_id]);

    sensor_data_config[sensor_data->id].type = type;
    sensor_data_config[sensor_data->id].voltage_enum = voltage_enum;
    
    if(sensor_data_config[sensor_data->id].type == PULSE_SENSOR) 
    {
        sensor_data_config[sensor_data->id].is_sensor_power_continuous = 1;
        /* If the sensor type is continuous, the power should be on all the time. */
        set_sensor_output(&sensor_power_configs[sensor_data->power_id], voltage_enum);
        // validate_output(&sensor_power_configs[sensor_data->power_id], voltage_enum, 10);
    }
    else
    {
        sensor_data_config[sensor_data->id].is_sensor_power_continuous = 0;
    }

    return 0;
}

int sensor_data_read(sensor_data_t *sensor_data, uint32_t timestamp)
{
    return 0;
}