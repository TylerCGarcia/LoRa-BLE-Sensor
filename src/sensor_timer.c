
#include <sensor_timer.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_TIMER, LOG_LEVEL_INF);

static uint32_t total_overflown_seconds = 0;

static void top_value_callback(const struct device *dev, void *user_data)
{
    LOG_DBG("Top value callback called");
    uint32_t top_value_seconds = counter_get_top_value(dev) * counter_get_frequency(dev);
    total_overflown_seconds += (top_value_seconds + 1);
}

int sensor_timer_init(const struct device *dev)
{
    int ret; 
    ret = sensor_timer_reset(dev);
    if (ret != 0) {
        LOG_ERR("Failed to reset counter for initialization");
        return ret;
    }
    const struct counter_top_cfg top_cfg = {
        .callback = top_value_callback,
        .ticks = counter_get_top_value(dev),
    };
    
    ret = counter_set_top_value(dev, &top_cfg);
    if (ret != 0) {
        LOG_ERR("Failed to set top value callback for initialization");
        return ret;
    }

    ret = sensor_timer_start(dev);
    if (ret != 0) {
        LOG_ERR("Failed to start counter for initialization");
        return ret;
    }
    return 0;
}

int sensor_timer_start(const struct device *dev)
{
    return counter_start(dev);
}

int sensor_timer_get_current_seconds(const struct device *dev)
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

int sensor_timer_get_total_seconds(const struct device *dev)
{
    int current_seconds = sensor_timer_get_current_seconds(dev);
    if (current_seconds < 0) {
        LOG_ERR("Failed to get current seconds");
        return -EINVAL;
    }
    return total_overflown_seconds + current_seconds;
}

int sensor_timer_reset(const struct device *dev)
{
    int ret;
    ret = counter_reset(dev);
    if (ret != 0) {
        LOG_ERR("Failed to reset counter");
        return ret;
    }
    total_overflown_seconds = 0;
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
    /* Make sure the alarm ticks are not greater than the top value of the timer */
    if(alarm_cfg.ticks > counter_get_top_value(dev)) {
        LOG_ERR("Alarm ticks are greater than the top value of the timer");
        return -EINVAL;
    }

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