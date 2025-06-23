#include "sensor_pmic.h"
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/charger.h>
#include <zephyr/drivers/mfd/npm1300.h>
#include <zephyr/drivers/sensor/npm1300_charger.h>

LOG_MODULE_REGISTER(SENSOR_PMIC, LOG_LEVEL_INF);

static int sensor_status_get(const struct device *dev, pmic_sensor_status_t *status);


static const struct device *pmic = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_pmic));
static const struct device *leds = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_leds));
static const struct device *charger = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_charger));

static int host_led_index = 2U;

// Default operations
static const sensor_pmic_ops_t default_ops = {
    .led_on = led_on,
    .led_off = led_off,
    .device_is_ready = device_is_ready,
    .sensor_status_get = sensor_status_get
};

// Current operations
static const sensor_pmic_ops_t *current_ops = &default_ops;

int sensor_pmic_init_with_ops(const sensor_pmic_ops_t *ops)
{
    LOG_INF("Initializing PMIC");
    if (ops != NULL) {
        current_ops = ops;
    }
    
    if (!current_ops->device_is_ready(pmic)) {
        LOG_ERR("PMIC device not ready");
        return -ENODEV;
    }
    if (!current_ops->device_is_ready(leds)) {
        LOG_ERR("LEDs device not ready");
        return -ENODEV;
    }
    if (!current_ops->device_is_ready(charger)) {
        LOG_ERR("Charger device not ready");
        return -ENODEV;
    }
    pmic_sensor_status_t status;
    if (sensor_pmic_status_get(&status) != 0) {
        LOG_ERR("Failed to get sensor attributes");
        return -EIO;
    }
    return 0;
}

int sensor_pmic_init(void)
{
    return sensor_pmic_init_with_ops(NULL);
}

int sensor_pmic_led_on(void)
{
    return current_ops->led_on(leds, host_led_index);
}

int sensor_pmic_led_off(void)
{
    return current_ops->led_off(leds, host_led_index);
}

int sensor_status_get(const struct device *dev, pmic_sensor_status_t *status)
{
    struct sensor_value value;
	int ret;

	ret = sensor_sample_fetch(charger);
	if (ret < 0) {
		return ret;
	}

	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_VOLTAGE, &value);
	status->voltage = (float)value.val1 + ((float)value.val2 / 1000000);
    LOG_DBG("Voltage: %f", status->voltage);
	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_TEMP, &value);
	status->temp = (float)value.val1 + ((float)value.val2 / 1000000);
    LOG_DBG("Temp: %f", status->temp);
	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_AVG_CURRENT, &value);
	status->current = (float)value.val1 + ((float)value.val2 / 1000000);
    LOG_DBG("Current: %f", status->current);

	sensor_channel_get(charger, (enum sensor_channel)SENSOR_CHAN_NPM1300_CHARGER_STATUS, &value);
	status->chg_status = value.val1;

	sensor_channel_get(charger, (enum sensor_channel)SENSOR_CHAN_NPM1300_CHARGER_ERROR, &value);
    status->chg_error = value.val1;

	sensor_attr_get(charger, (enum sensor_channel)SENSOR_CHAN_NPM1300_CHARGER_VBUS_STATUS, (enum sensor_attribute)SENSOR_ATTR_NPM1300_CHARGER_VBUS_PRESENT, &value);
    status->vbus_present = value.val1;
    LOG_DBG("Charger Status: %d, Error: %d, VBUS: %s\n", status->chg_status, status->chg_error, status->vbus_present ? "connected" : "disconnected");
	return 0;
}

int sensor_pmic_status_get(pmic_sensor_status_t *status)
{
	return current_ops->sensor_status_get(charger, status);
}

// typedef struct {
//     float voltage;
//     float current;
//     float temp;
//     int32_t chg_status;
// } pmic_sensor_status_t;

// static int read_sensors(const struct device *charger, sensor_status_t *status)
// {
// 	struct sensor_value value;
// 	int ret;

// 	ret = sensor_sample_fetch(charger);
// 	if (ret < 0) {
// 		return ret;
// 	}

// 	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_VOLTAGE, &value);
// 	*voltage = (float)value.val1 + ((float)value.val2 / 1000000);

// 	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_TEMP, &value);
// 	*temp = (float)value.val1 + ((float)value.val2 / 1000000);

// 	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_AVG_CURRENT, &value);
// 	*current = (float)value.val1 + ((float)value.val2 / 1000000);

// 	sensor_channel_get(charger, SENSOR_CHAN_NPM1300_CHARGER_STATUS, &value);
// 	*chg_status = value.val1;

// 	return 0;
// }