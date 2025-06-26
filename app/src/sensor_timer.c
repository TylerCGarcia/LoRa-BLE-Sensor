
#include <sensor_timer.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_TIMER, LOG_LEVEL_INF);

static uint32_t total_overflown_seconds = 0;

static void top_value_callback(const struct device *dev, void *user_data)
{
    LOG_DBG("Top value callback called");
    uint32_t top_value_seconds = counter_get_top_value(dev) / counter_get_frequency(dev);
    total_overflown_seconds += (top_value_seconds + 1);
}

int sensor_timer_init(const struct device *dev)
{
    int ret; 
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
    uint32_t ticks;
    ret = counter_get_value(dev, &ticks);
    if (ret != 0) {
        return ret;
    }
    return (int)(ticks / counter_get_frequency(dev));
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
    
    /* Stop the counter */
    ret = counter_stop(dev);
    if (ret != 0) {
        LOG_ERR("Failed to stop counter");
        return ret;
    }
    
    /* Wait a bit to ensure counter is fully stopped */
    k_sleep(K_MSEC(1));
    
    /* Reinitialize the counter with the same top value */
    const struct counter_top_cfg top_cfg = {
        .callback = top_value_callback,
        .ticks = counter_get_top_value(dev),
    };
    
    ret = counter_set_top_value(dev, &top_cfg);
    if (ret != 0) {
        LOG_ERR("Failed to set top value for reset");
        return ret;
    }
    
    /* Restart the counter */
    ret = counter_start(dev);
    if (ret != 0) {
        LOG_ERR("Failed to restart counter");
        return ret;
    }
    
    /* Reset our overflow tracking */
    total_overflown_seconds = 0;
    
    return 0;
}

int sensor_timer_stop(const struct device *dev)
{
    return counter_stop(dev);
}

// Replace the entire function from line 111 to 128 with:
int sensor_timer_set_alarm(const struct device *dev, sensor_timer_alarm_cfg_t *sensor_timer_alarm_cfg)
{
    int ret;
    uint32_t alarm_ticks;
    
    // Convert seconds to ticks by multiplying by frequency
    alarm_ticks = sensor_timer_alarm_cfg->alarm_seconds * counter_get_frequency(dev);
    
    struct counter_alarm_cfg alarm_cfg = {
        .callback = sensor_timer_alarm_cfg->callback,
        .ticks = alarm_ticks,
        .flags = 0,  // Relative alarm
    };
    
    /* Make sure the alarm ticks are not greater than the top value of the timer */
    if (alarm_ticks > counter_get_top_value(dev)) {
        LOG_ERR("Alarm ticks (%u) are greater than the top value (%u) of the timer", 
                alarm_ticks, counter_get_top_value(dev));
        return -EINVAL;
    }

    sensor_timer_alarm_cfg->is_alarm_set = 1;
    ret = counter_set_channel_alarm(dev, sensor_timer_alarm_cfg->channel, &alarm_cfg);
    if (ret != 0) {
        return ret;
    }
    
    return 0;  // Add missing return statement
}

int sensor_timer_cancel_alarm(const struct device *dev, uint8_t channel)
{
    return counter_cancel_channel_alarm(dev, channel);
}