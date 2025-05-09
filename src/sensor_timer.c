
#include <sensor_timer.h>
#include <zephyr/drivers/counter.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_TIMER, LOG_LEVEL_INF);

int sensor_timer_init(const struct device *dev)
{
    int ret;
    ret = counter_start(dev);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

uint32_t sensor_timer_get_seconds(const struct device *dev)
{
    LOG_INF("Timer Frequency: %d", counter_get_frequency(dev));
    uint32_t time;
    counter_get_value(dev, &time);
    return time / counter_get_frequency(dev);
}
