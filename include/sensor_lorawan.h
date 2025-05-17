/**
 * @file sensor_lorawan.h
 * @author Tyler Garcia
 * @brief This is a library to handle the LoRaWAN functionality for the sensor.
 * @version 0.1
 * @date 2025-05-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SENSOR_LORAWAN_H
#define SENSOR_LORAWAN_H

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

/**
 * @brief Check if the LoRaWAN Network is configured with valid lorawan keys.
 * 
 * @param setup setup configuration used to join lorawan network
 * @return int 1 if configured, 0 if not configured
 */
int is_lorawan_configured(lorawan_setup_t *setup);

/**
 * @brief Print the LoRaWAN Network dev_eui, join_eui, and app_key for a given lorawan_setup_t configuration. 
 * 
 * @param setup setup configuration used to join lorawan network
 */
void sensor_lorawan_log_network_config(lorawan_setup_t *setup);

/**
 * @brief Setup LoRaWAN Network with a given lorawan_setup_t configuration. 
 * 
 * @param setup setup configuration used to join lorawan network
 * @return int 0 if successful connection, < 0 if unsuccessful
 */
int sensor_lorawan_setup(lorawan_setup_t *setup);

/**
 * @brief Send data to the LoRaWAN Network.
 * 
 * @param data lorawan_data_t structure containing the data to send.
 * @return int 0 if successful, -1 if failed.
 */
int sensor_lorawan_send_data(lorawan_data_t *data);

/**
 * @brief Check if the LoRaWAN Network is connected.
 * 
 * @return int 1 if connected, 0 if not connected
 */
int is_lorawan_connected(void);

#endif

