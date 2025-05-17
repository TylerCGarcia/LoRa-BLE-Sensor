// In tests/fakes/sensor_lorawan_fakes.h
#ifndef SENSOR_LORAWAN_FAKES_H
#define SENSOR_LORAWAN_FAKES_H

#include <zephyr/fff.h>
#include <zephyr/lorawan/lorawan.h>
#include <stdint.h>
#include <zephyr/device.h>

#define MAX_LORAWAN_PAYLOAD 255

/**
 * @brief Structure to hold the LoRaWAN setup, used to join the network.
 */
typedef struct {
    /* LoRaWAN Device. */
    const struct device *lora_dev;
    /* LoRaWAN uplink class. */
    enum lorawan_class uplink_class;
    /* LoRaWAN downlink callback. */
    struct lorawan_downlink_cb downlink_callback;
    /* How many attempts to join the network. If 0, it will attempt to join indefinitely. */
    int join_attempts;
    /* Device Nonce for LoRaWAN Network. Should be tracked seperately for different euis. */
    uint16_t dev_nonce;
    /* Delay between joinattempts in milliseconds. */
    uint32_t delay;
    /* Device EUI for LoRaWAN Network. */
    uint8_t dev_eui[8];
    /* Join EUI for LoRaWAN Network. */
    uint8_t join_eui[8];
    /* App Key for LoRaWAN Network. */
    uint8_t app_key[16];

} lorawan_setup_t;

/**
 * @brief Structure to hold the LoRaWAN data to send.
 */
typedef struct {
    /* Data to send. */
    uint8_t data[MAX_LORAWAN_PAYLOAD];
    /* Length of the data to send. */
    uint16_t length;
    /* LoRaWAN port to send the data to. */
    uint8_t port;
    /* How many attempts to send data when waiting for an ack, if 0, it will send an unconfirmed message. */
    uint8_t attempts;
    /* Delay between attempts in milliseconds. */
    uint32_t delay;
} lorawan_data_t;

// Declare fake functions
DECLARE_FAKE_VALUE_FUNC(int, is_lorawan_configured, lorawan_setup_t*);
DECLARE_FAKE_VOID_FUNC(lorawan_log_network_config, lorawan_setup_t*);
DECLARE_FAKE_VALUE_FUNC(int, lorawan_setup, lorawan_setup_t*);
DECLARE_FAKE_VALUE_FUNC(int, lorawan_send_data, lorawan_data_t*);
DECLARE_FAKE_VALUE_FUNC(int, is_lorawan_connected);

// Reset all fakes
void sensor_lorawan_fakes_reset(void);

#endif // SENSOR_LORAWAN_FAKES_H