#include <zephyr/fff.h>

DEFINE_FAKE_VALUE_FUNC(int, regulator_fake_enable, const struct device *);
DEFINE_FAKE_VALUE_FUNC(int, regulator_fake_disable, const struct device *);
DEFINE_FAKE_VALUE_FUNC(int, regulator_fake_count_voltages, const struct device *);
DEFINE_FAKE_VALUE_FUNC(int, regulator_fake_list_voltage, const struct device *, unsigned int);
DEFINE_FAKE_VALUE_FUNC(int, regulator_fake_set_voltage, const struct device *, int32_t, int32_t);