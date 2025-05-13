#include "sensor_scheduling.h"
#include "sensor_timer.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(SENSOR_SCHEDULING, LOG_LEVEL_INF);

static const struct device *scheduling_timer;

static sensor_scheduling_cfg_t *scheduling_list[SENSOR_SCHEDULING_ID_LIMIT];

/* The channel id of the alarm for each scheduling id */
static uint8_t scheduling_channels[SENSOR_SCHEDULING_ID_LIMIT];
static uint8_t channel_frequencies[SENSOR_SCHEDULING_ID_LIMIT];



// static int determine_channel_for_schedule(sensor_scheduling_cfg_t *schedule)
// {
//     // First check if there's an existing alarm with the same frequency
//     for (int i = 0; i < SENSOR_SCHEDULING_ID_LIMIT; i++) {
//         LOG_DBG("Checking schedule %d with frequency %d", i, scheduling_list[i]->frequency_seconds);
//         if (scheduling_list[i] != NULL && scheduling_list[i]->frequency_seconds == schedule->frequency_seconds) {
//             // Found a schedule with same frequency, share its channel
//             scheduling_channels[schedule->id] = scheduling_channels[i];
//             return scheduling_channels[i];
//         }
//     }
//     return -1;
// }

static void scheduling_callback(const struct device *dev, uint8_t chan_id, uint32_t ticks)
{
    LOG_INF("Scheduling callback called for channel %d", chan_id);
    for(int i = 0; i < SENSOR_SCHEDULING_ID_LIMIT; i++) 
    {
        if(scheduling_list[i] != NULL) 
        {
            if(scheduling_channels[i] == chan_id && scheduling_list[i]->is_scheduled) {
                scheduling_list[i]->is_triggered = 1;
                scheduling_list[i]->last_event_time = sensor_timer_get_total_seconds(scheduling_timer);
            }
        }
    }
}

/**
 * @brief Add an alarm to a schedule, as of right now, the channel is the same as the schedule id
 * 
 * @param schedule 
 */
static void add_alarm_to_schedule(sensor_scheduling_cfg_t *schedule)
{
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .channel = schedule->id,
        .alarm_seconds = schedule->frequency_seconds,
        .callback = scheduling_callback,
        .is_alarm_set = 0,
    };
    scheduling_channels[schedule->id] = alarm_cfg.channel;
    sensor_timer_set_alarm(scheduling_timer, &alarm_cfg);
}

/**
 * @brief Renew an alarm for a schedule, this is called when the schedule is reset
 * 
 * @param schedule the schedule to renew the alarm for
 * @param time_to_next_event the time till the next alarm should go off
 * @return int 0 on success, -EINVAL on failure
 */
static int renew_alarm_for_schedule(sensor_scheduling_cfg_t *schedule, uint32_t time_to_next_event)
{
    if (schedule == NULL || !schedule->is_scheduled) {
        return -EINVAL;
    }
    LOG_INF("Renewing alarm for schedule %d", schedule->id);
    LOG_DBG("current time: %d", sensor_timer_get_total_seconds(scheduling_timer));
    LOG_DBG("Time of last event: %d", schedule->last_event_time);
    LOG_DBG("Time to next event: %d", time_to_next_event);
    sensor_timer_alarm_cfg_t alarm_cfg = {
        .channel = scheduling_channels[schedule->id],
        .alarm_seconds = time_to_next_event,
        .callback = scheduling_callback,
        .is_alarm_set = 0,
    };
    return sensor_timer_set_alarm(scheduling_timer, &alarm_cfg);
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
    sensor_timer_cancel_alarm(scheduling_timer, &scheduling_channels[schedule->id]);
    schedule->is_scheduled = 0;
    schedule->is_triggered = 0;
    return 0;
}

int sensor_scheduling_reset_schedule(sensor_scheduling_cfg_t *schedule)
{
    /* Reset the schedule */
    schedule->is_triggered = 0;
    uint32_t time_to_next_event;
    int current_time = sensor_timer_get_total_seconds(scheduling_timer);
    if(current_time < 0)
    {
        LOG_ERR("Failed to get current time for renewing schedule");
        return -EINVAL;
    }
    if(current_time - schedule->last_event_time < schedule->frequency_seconds)
    {
        time_to_next_event = schedule->frequency_seconds - (current_time - schedule->last_event_time);
    }
    else
    {
        LOG_ERR("Failed to reset schedule because the time since the last event is more than the frequency");
        time_to_next_event = schedule->frequency_seconds;
    }

    return renew_alarm_for_schedule(schedule, time_to_next_event);
}
