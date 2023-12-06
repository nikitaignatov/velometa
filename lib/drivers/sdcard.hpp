#ifndef _VH_DRIVERS_SDCARD_HPP_
#define _VH_DRIVERS_SDCARD_HPP_

#include "types.hpp"

static const char *TAG = "display";
#define MOUNT_POINT "/sdcard"
#include "config.hpp"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <ff.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "FFat.h"
// SD -- start
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Modify CS_PIN for your chip select pin.
#define CS_PIN SS
// SD -- end

// // SPI - ESP32-S3
#define SDSPI_HOST_ID SPI3_HOST
#define SD_MISO GPIO_NUM_38
#define SD_MOSI GPIO_NUM_40
#define SD_SCLK GPIO_NUM_39
#define SD_CS GPIO_NUM_41
static sdmmc_card_t *sdcard;
bool init_sdspi();
FATFS *fs_mount_sd_card();
void write_task_code(void *parameter);
#endif