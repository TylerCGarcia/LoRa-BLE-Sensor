#include <sensor_pmic.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>


LOG_MODULE_REGISTER(SENSOR_PMIC, LOG_LEVEL_INF);

// static const struct device *charger = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_charger));

static const struct device *pmic = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_pmic));
static const struct device *leds = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_leds));
// static const struct device *pmic = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_pmic));

static int host_led_index = 2U;

// In your header file (e.g., sensor_utils.h)
// __attribute__((weak)) bool pmic_device_is_ready(const struct device *dev)
// {
//     return device_is_ready(dev);
// }

bool pmic_device_is_ready(const struct device *dev)
{
    return device_is_ready(dev);
}

int sensor_pmic_init(void)
{
    LOG_INF("Initializing PMIC");
    if (!pmic_device_is_ready(pmic)) {
        LOG_ERR("LEDs device not ready");
        return -ENODEV;
    }
    return 0;
}

int sensor_pmic_led_on(void)
{
    return led_on(leds, host_led_index);
}

int sensor_pmic_led_off(void)
{
    return led_off(leds, host_led_index);
}