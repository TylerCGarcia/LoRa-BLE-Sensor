#include <zephyr/ztest.h>
#include <zephyr/logging/log.h>
#include "sensor_pmic.h"
#include "sensor_led_fakes.h"

LOG_MODULE_REGISTER(tests_pmic, LOG_LEVEL_INF);

// Create mock operations with wrapper functions
sensor_pmic_ops_t mock_ops = {
    .led_on = led_on_wrapper,
    .led_off = led_off_wrapper,
    .device_is_ready = device_is_ready_wrapper,
    .sensor_status_get = sensor_status_get_wrapper
};

ZTEST_SUITE(pmic, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test that the PMIC can be initialized
 * 
 */
ZTEST(pmic, test_pmic_init)
{

    int ret = sensor_pmic_init_with_ops(&mock_ops);
    zassert_ok(ret, "Failed to initialize PMIC");
}

/**
 * @brief Test that the PMIC can turn on the LED
 * 
 */
ZTEST(pmic, test_pmic_led_on)
{
    int ret = sensor_pmic_init_with_ops(&mock_ops);
    zassert_ok(ret, "Failed to initialize PMIC");

    ret = sensor_pmic_led_on();
    zassert_ok(ret, "Failed to turn on LED");
}

/**
 * @brief Test that the PMIC can turn off the LED
 * 
 */
ZTEST(pmic, test_pmic_led_off)
{
    int ret = sensor_pmic_init_with_ops(&mock_ops);
    zassert_ok(ret, "Failed to initialize PMIC");

    ret = sensor_pmic_led_off();
    zassert_ok(ret, "Failed to turn off LED");
}

ZTEST(pmic, test_pmic_sensor_attr_get)
{
    int ret = sensor_pmic_init_with_ops(&mock_ops);
    zassert_ok(ret, "Failed to initialize PMIC");

}
