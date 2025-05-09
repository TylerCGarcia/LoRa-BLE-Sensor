
#include <sensor_timer.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_TIMER, LOG_LEVEL_INF);

int sensor_timer_init(const struct device *dev)
{
    return counter_start(dev);
}

int sensor_timer_get_seconds(const struct device *dev)
{
    int ret;
    LOG_DBG("Timer Frequency: %d", counter_get_frequency(dev));
    int time;
    ret = counter_get_value(dev, &time);
    if (ret != 0) {
        return ret;
    }
    return time / counter_get_frequency(dev);
}

int sensor_timer_reset(const struct device *dev)
{
    int ret;
    ret = counter_reset(dev);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

int sensor_timer_stop(const struct device *dev)
{
    return counter_stop(dev);
}

int sensor_timer_set_alarm(const struct device *dev, sensor_timer_alarm_cfg_t *sensor_timer_alarm_cfg)
{
    struct counter_alarm_cfg alarm_cfg = {
        .callback = sensor_timer_alarm_cfg->callback,
        .ticks = (sensor_timer_alarm_cfg->alarm_seconds * counter_get_frequency(dev))
    };
    return counter_set_channel_alarm(dev, 0, &alarm_cfg);
}