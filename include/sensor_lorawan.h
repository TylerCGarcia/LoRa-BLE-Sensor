#ifndef SENSOR_LORAWAN_H
#define SENSOR_LORAWAN_H

typedef struct {
    /* LoRaWAN Device. */
    const struct device *lora_dev;
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


#endif

