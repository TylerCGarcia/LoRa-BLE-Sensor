
#include "sensor_power_fakes.h"


DECLARE_FAKE_VALUE_FUNC(int, sensor_pmic_init);
DECLARE_FAKE_VALUE_FUNC(int, sensor_pmic_led_on);
DECLARE_FAKE_VALUE_FUNC(int, sensor_pmic_led_off);

// Reset all fakes
void sensor_pmic_fakes_reset(void);