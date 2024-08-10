#include "esp_log.h"
#include "config.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <Arduino.h>
#include "types.hpp"
#include "ble.hpp"
#include "gps.hpp"
#include "mock_data.hpp"
#include "activity.hpp"
#include <ff.h>
#include "sensor_reader.hpp"

RTC_DATA_ATTR int start_count = 0;

uint16_t map_frequency = 1000;
SemaphoreHandle_t vh_display_semaphore;
SemaphoreHandle_t vm_sdcard_semaphor;

FATFS *fatfs;
#include "wt32sc01plus.hpp"

TaskHandle_t ble_task;
TaskHandle_t sensor_reader;
TaskHandle_t display_task;
TaskHandle_t activity_task;
TaskHandle_t gps_task;

std::vector<sensor_definition_t> ble_sensors;
QueueHandle_t vh_raw_measurement_queue;
QueueHandle_t vh_metrics_queue;
QueueHandle_t vh_gps_csv_queue;
QueueHandle_t vh_gps_queue;
QueueHandle_t vm_csv_queue;

EventGroupHandle_t sensor_status_bits;

static const char *TAG = "main.cpp";

void setup()
{
    Serial.begin(115200);
    delay(5000);
    Serial.println("Velometa");
    sensor_status_bits = xEventGroupCreate();

    vh_display_semaphore = xSemaphoreCreateMutex();
    if (vh_display_semaphore == NULL)
    {
        ESP_LOGE(TAG, "Failed to create display semaphore");
    }

    vh_gps_queue = xQueueCreate(1000, sizeof(gps_data_t));
    if (vh_gps_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create vh_gps_queue");
    }

    vm_csv_queue = xQueueCreate(200, sizeof(raw_measurement_msg_t));
    if (vm_csv_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create vm_csv_queue");
    }

    vh_gps_csv_queue = xQueueCreate(400, sizeof(gps_data_t));
    if (vh_gps_csv_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create vh_gps_csv_queue");
    }

    vm_sdcard_semaphor = xSemaphoreCreateMutex();
    vh_raw_measurement_queue = xQueueCreate(200, sizeof(raw_measurement_msg_t));

    if (psramInit())
    {
        ESP_LOGW(TAG, "\nThe PSRAM is correctly initialized");
        ESP_LOGW(TAG, "PSRAM Size available (bytes): %d", ESP.getFreePsram());
    }
    else
    {
        ESP_LOGE(TAG, "\nPSRAM does not work");
    }

    fatfs = fs_mount_sd_card();
    Serial.print(fatfs->fsize);
    Serial.println(" init_sdspi");

    xTaskCreatePinnedToCore(
        display_task_code, /* Function to implement the task */
        "display_task",    /* Name of the task */
        32 * 1024,         /* Stack size in words */
        NULL,              /* Task input parameter */
        0,                 /* Priority of the task */
        &display_task,     /* Task handle. */
        1);                /* Core where the task should run */
        
    // PSRAM Initialisation
    if (psramInit())
    {
        Serial.println("\nThe PSRAM is correctly initialized");
        Serial.println((String) "PSRAM Size available (bytes): " + ESP.getFreePsram());
    }
    else
    {
        Serial.println("\nPSRAM does not work");
    }

    ble_sensors.push_back((sensor_definition_t){
        .device_name = DEVICE_NAME_HR,
        .metric = metric_type_t::HR_BPM,
        .service_id = BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = {{measurement_t::heartrate, BLEUUID((uint16_t)0x2A37)}},
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_hr_data,
        .enabled = false,
        .address_type = esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM,
        .has_notification = true,
    });

    ble_sensors.push_back((sensor_definition_t){
        .device_name = DEVICE_NAME_HR2,
        .metric = metric_type_t::HR_BPM,
        .service_id = BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = {{measurement_t::heartrate, BLEUUID((uint16_t)0x2A37)}},
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_hr_data,
        .enabled = false,
        .address_type = esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM,
        .has_notification = true,
    });

    ble_sensors.push_back((sensor_definition_t){
        .device_name = DEVICE_NAME_POWER,
        .metric = metric_type_t::POWER_WATT,
        .service_id = BLEUUID("00001818-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = {{measurement_t::power, BLEUUID((uint16_t)0x2A63)}},
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_power_watt_data,
        .enabled = false,
        .address_type = esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM,
        .has_notification = true,
    });

    // xTaskCreatePinnedToCore(
    //     mock_task_code, /* Function to implement the task */
    //     "mock_task",    /* Name of the task */
    //     8 * 1024,       /* Stack size in words */
    //     NULL,           /* Task input parameter */
    //     0,              /* Priority of the task */
    //     NULL,           /* Task handle. */
    //     0);             /* Core where the task should run */

    xTaskCreatePinnedToCore(
        write_task_code, /* Function to implement the task */
        "write_task",    /* Name of the task */
        8 * 1024,        /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        NULL,            /* Task handle. */
        0);              /* Core where the task should run */

    xTaskCreatePinnedToCore(
        ble_task_code,   /* Function to implement the task */
        "ble_task_code", /* Name of the task */
        16 * 1024,       /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &ble_task,       /* Task handle. */
        0);              /* Core where the task should run */

    xTaskCreatePinnedToCore(
        sensor_reader_task_code, /* Function to implement the task */
        "sensor_reader",         /* Name of the task */
        4 * 1024,                /* Stack size in words */
        NULL,                    /* Task input parameter */
        0,                       /* Priority of the task */
        &sensor_reader,          /* Task handle. */
        0);                      /* Core where the task should run */

    xTaskCreatePinnedToCore(
        activity_task_code,   /* Function to implement the task */
        "activity_task_code", /* Name of the task */
        24 * 1024,            /* Stack size in words */
        NULL,                 /* Task input parameter */
        0,                    /* Priority of the task */
        &activity_task,       /* Task handle. */
        0);                   /* Core where the task should run */

#ifdef FEATURE_GPS_ENABLED
    xTaskCreatePinnedToCore(
        gps_task_code,   /* Function to implement the task */
        "gps_task_code", /* Name of the task */
        4 * 1024,        /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &gps_task,       /* Task handle. */
        0);              /* Core where the task should run */

    xTaskCreatePinnedToCore(
        gps_process_task_code,   /* Function to implement the task */
        "gps_process_task_code", /* Name of the task */
        4 * 1024,                /* Stack size in words */
        NULL,                    /* Task input parameter */
        0,                       /* Priority of the task */
        NULL,                    /* Task handle. */
        0);                      /* Core where the task should run */
#endif

    vTaskDelete(NULL);
}

void loop()
{
}