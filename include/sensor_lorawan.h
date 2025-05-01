#ifndef SENSOR_LORAWAN_H
#define SENSOR_LORAWAN_H

#include <zephyr/lorawan/lorawan.h>
#include <stdint.h>
#include <zephyr/device.h>

typedef struct {
    /* LoRaWAN Device. */
    const struct device *lora_dev;
    /* LoRaWAN downlink callback. */
    struct lorawan_downlink_cb downlink_callback;
    /* How many attempts to join the network. */
    int join_attempts;
    /* Device EUI for LoRaWAN Network. */
    uint8_t dev_eui[8];
    /* Join EUI for LoRaWAN Network. */
    uint8_t join_eui[8];
    /* App Key for LoRaWAN Network. */
    uint8_t app_key[16];

} lorawan_setup_t;

/**
 * @brief Setup LoRaWAN Network with a given lorawan_setup_t configuration. 
 * 
 * @param setup 
 * @return int 
 */
int lorawan_setup(lorawan_setup_t *setup);

/**
 * @brief Check if the LoRaWAN Network is connected.
 * 
 * @return int 1 if connected, 0 if not connected
 */
int is_lorawan_connected(void);

#endif

