// In tests/fakes/sensor_pmic_fakes.h
#ifndef SENSOR_PMIC_FAKES_H
#define SENSOR_PMIC_FAKES_H

#include <zephyr/fff.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float percentage;
    float voltage;
    float current;
    float temp;
    uint32_t chg_status;
    uint32_t chg_error;
    bool vbus_present;
} pmic_sensor_status_t;

DECLARE_FAKE_VALUE_FUNC(int, sensor_pmic_init);
DECLARE_FAKE_VALUE_FUNC(int, sensor_pmic_led_on);
DECLARE_FAKE_VALUE_FUNC(int, sensor_pmic_led_off);
DECLARE_FAKE_VALUE_FUNC(int, sensor_pmic_status_get, pmic_sensor_status_t *);

// Reset all fakes
void sensor_pmic_fakes_reset(void);

#endif