

#include "sensor_nvs.h"
#include <zephyr/logging/log.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/drivers/flash.h>

LOG_MODULE_REGISTER(SENSOR_NVS, LOG_LEVEL_INF);

static struct nvs_fs fs;

static uint8_t sector_count = 2U;

#define NVS_PARTITION		    storage_partition
#define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)

int sensor_nvs_setup(void)
{
    int ret;
    struct flash_pages_info info;
    fs.flash_device = NVS_PARTITION_DEVICE;
    if (!device_is_ready(fs.flash_device)) {
        LOG_ERR("Flash device %s is not ready", fs.flash_device->name);
        return -1;
    }
    fs.offset = NVS_PARTITION_OFFSET;
    ret = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
    if (ret) {
        LOG_ERR("Unable to get page info, error: %d", ret);
        return -1;
    }
    fs.sector_size = info.size;
    LOG_INF("Sector size: %d", fs.sector_size);
    fs.sector_count = sector_count;
    ret = nvs_mount(&fs);
    if (ret) {
        LOG_ERR("Flash Init failed, error: %d", ret);
        return -1;
    }
    return 0;
}

int sensor_nvs_write(enum sensor_nvs_address address, const void *data, size_t length)
{
    int ret;
    if (length > SENSOR_NVS_MAX_SIZE) {
        LOG_ERR("Data length is too large, max size is %d", SENSOR_NVS_MAX_SIZE);
        return -1;
    }
    ret = nvs_write(&fs, address, data, length);
    if (ret != length) {
        LOG_ERR("Failed to write to NVS, error: %d", ret);
        return -1;
    }
    return 0;
}

int sensor_nvs_read(enum sensor_nvs_address address, void *data, size_t length)
{
    int ret;
    if (length > SENSOR_NVS_MAX_SIZE) {
        LOG_ERR("Data length is too large, max size is %d", SENSOR_NVS_MAX_SIZE);
        return -1;
    }
    ret = nvs_read(&fs, address, data, length);
    if (ret != length) {
        LOG_ERR("Failed to read from NVS, error: %d", ret);
        return -1;
    }
    return 0;
}

int sensor_nvs_delete(enum sensor_nvs_address address)
{
    return nvs_delete(&fs, address);
}

int sensor_nvs_clear(void)
{
    return nvs_clear(&fs);
}