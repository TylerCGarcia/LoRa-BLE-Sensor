#ifndef SENSOR_LORAWAN_H
#define SENSOR_LORAWAN_H

#include <zephyr/lorawan/lorawan.h>
#include <stdint.h>
#include <zephyr/device.h>

typedef struct {
    /* LoRaWAN Device. */
    const struct device *lora_dev;
    /* LoRaWAN uplink class. */
    enum lorawan_class uplink_class;
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


typedef struct {
    /* Data to send. */
    uint8_t *data;
    /* Length of the data to send. */
    uint16_t length;
    /* LoRaWAN port to send the data to. */
    uint8_t port;
    /* How many attempts to send data when waiting for an ack, if 0, it will send an unconfirmed message. */
    uint8_t attempts;
    /* Delay between attempts in milliseconds. */
    uint32_t delay;
} lorawan_data_t;


/**
 * @brief Setup LoRaWAN Network with a given lorawan_setup_t configuration. 
 * 
 * @param setup 
 * @return int 
 */
int lorawan_setup(lorawan_setup_t *setup);

/**
 * @brief Send data to the LoRaWAN Network.
 * 
 * @param data lorawan_data_t structure containing the data to send.
 * @return int 0 if successful, -1 if failed.
 */
int lorawan_send_data(lorawan_data_t *data);

/**
 * @brief Check if the LoRaWAN Network is connected.
 * 
 * @return int 1 if connected, 0 if not connected
 */
int is_lorawan_connected(void);

#endif

