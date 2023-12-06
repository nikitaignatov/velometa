#ifndef _VH_DRIVERS_SDCARD_HPP_
#define _VH_DRIVERS_SDCARD_HPP_

static const char *TAG = "display";
#define MOUNT_POINT "/sdcard"
#include "config.hpp"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <ff.h>

// // SPI - ESP32-S3
#define SDSPI_HOST_ID SPI3_HOST
#define SD_MISO GPIO_NUM_38
#define SD_MOSI GPIO_NUM_40
#define SD_SCLK GPIO_NUM_39
#define SD_CS GPIO_NUM_41
static sdmmc_card_t *sdcard;
bool init_sdspi();
FATFS *fs_mount_sd_card();

#endif