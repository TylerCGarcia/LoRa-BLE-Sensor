#include "sensor_scheduling.h"
#include "sensor_timer.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_SCHEDULING, LOG_LEVEL_INF);

static const struct device *scheduling_timer;

static sensor_scheduling_cfg_t *scheduling_list[SENSOR_SCHEDULING_ID_LIMIT];

/* The channel id of the alarm for each scheduling id */
static uint8_t scheduling_channels[SENSOR_SCHEDULING_ID_LIMIT];
static uint8_t channel_frequencies[SENSOR_SCHEDULING_ID_LIMIT];

static void scheduling_callback(const struct device *dev, uint8_t chan_id, uint32_t ticks)
{
    LOG_INF("Scheduling callback called for channel %d", chan_id);
    for(int i = 0; i < SENSOR_SCHEDULING_ID_LIMIT; i++) 
    {
        if(scheduling_list[i] != NULL) 
        {
            if(scheduling_channels[i] == chan_id && scheduling_list[i]->is_scheduled) {
                scheduling_list[i]->is_triggered = 1;
            }
        }
    }
}

static void add_alarm_to_schedule(sensor_scheduling_cfg_t *schedule)
{
    // First check if there's an existing alarm with the same frequency
    // for (int i = 0; i < SENSOR_SCHEDULING_ID_LIMIT; i++) {
    //     LOG_DBG("Checking schedule %d with frequency %d", i, scheduling_list[i]->frequency_seconds);
    //     if (scheduling_list[i] != NULL && scheduling_list[i]->frequency_seconds == schedule->frequency_seconds) {
    //         // Found a schedule with same frequency, share its channel
    //         scheduling_channels[schedule->id] = scheduling_channels[i];
    //         return;  // No need to create new alarm
    //     }
    // }
    /* If we get here, no matching frequency was found, create new alarm with lowest channel not set. */
    // uint8_t channel = 0;
    // while(scheduling_channels[channel] != 0) 
    // {
    //     channel++;
    // }
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .channel = schedule->id,
        .alarm_seconds = schedule->frequency_seconds,
        .callback = scheduling_callback,
        .is_alarm_set = 0,
    };
    scheduling_channels[schedule->id] = alarm_cfg.channel;
    sensor_timer_set_alarm(scheduling_timer, &alarm_cfg);
}

int sensor_scheduling_init(const struct device *timer)
{
    int ret;
    scheduling_timer = timer;
    ret = sensor_timer_init(scheduling_timer);
    if (ret != 0) {
        LOG_ERR("Failed to initialize scheduling timer");
        return ret;
    }
    return 0;
}

int sensor_scheduling_add_schedule(sensor_scheduling_cfg_t *schedule)
{
    /* Set the schedule as scheduled */
    schedule->is_scheduled = 1;
    /* Add the schedule to the local list of schedules */
    scheduling_list[schedule->id] = schedule;
    /* Add the schedule to an alarm */
    add_alarm_to_schedule(schedule);
    return 0;
}

int sensor_scheduling_remove_schedule(sensor_scheduling_cfg_t *schedule)
{
    /* Remove the schedule from the local list of schedules */
    // scheduling_list[schedule->id] = NULL;
    schedule->is_scheduled = 0;
    return 0;
}

