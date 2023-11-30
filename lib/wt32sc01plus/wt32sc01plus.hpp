#ifndef _VH_WT32SC01_HPP_
#define _VH_WT32SC01_HPP_

static const char *TAG = "display";
#define MOUNT_POINT "/sdcard"

#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
// // SPI - ESP32-S3
#define SDSPI_HOST_ID SPI3_HOST
#define SD_MISO GPIO_NUM_38
#define SD_MOSI GPIO_NUM_40
#define SD_SCLK GPIO_NUM_39
#define SD_CS GPIO_NUM_41

static sdmmc_card_t *sdcard;
extern uint16_t map_frequency;
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <float.h>

#include "types.hpp"

#define MSG_NEW_SPEED 1
#define MSG_NEW_POWER 2
#define MSG_NEW_HR 3
#define MSG_NEW_HEADING 4

extern system_t sys;
void vh_setup();
bool init_sdspi();
void vh_loop();
void update(system_t sys);
void publish(uint32_t topic, metric_info_t payload);

#endif