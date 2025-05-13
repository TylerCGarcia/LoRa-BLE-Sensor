
#include <sensor_timer.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_TIMER, LOG_LEVEL_INF);

int sensor_timer_start(const struct device *dev)
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
    int ret;
    struct counter_alarm_cfg alarm_cfg = {
        .callback = sensor_timer_alarm_cfg->callback,
        .ticks = (sensor_timer_alarm_cfg->alarm_seconds * counter_get_frequency(dev))
    };
    sensor_timer_alarm_cfg->is_alarm_set = 1;
    ret = counter_set_channel_alarm(dev, sensor_timer_alarm_cfg->channel, &alarm_cfg);
    if (ret != 0) {
        return ret;
    }
}

int sensor_timer_cancel_alarm(const struct device *dev, sensor_timer_alarm_cfg_t *sensor_timer_alarm_cfg)
{
    sensor_timer_alarm_cfg->is_alarm_set = 0;
    return counter_cancel_channel_alarm(dev, sensor_timer_alarm_cfg->channel);
}