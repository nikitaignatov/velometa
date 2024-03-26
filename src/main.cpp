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

RTC_DATA_ATTR int start_count = 0;

uint16_t map_frequency = 1000;
SemaphoreHandle_t vh_display_semaphore;
SemaphoreHandle_t vm_sdcard_semaphor;

FATFS *fatfs;
#if USE_EPAPER
#include "display_420.hpp"
#elif USE_LCD
#include "wt32sc01plus.hpp"
#endif

TaskHandle_t ble_task;
TaskHandle_t airspeed_task;
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
    delay(50);
    Serial.println("Velometa");

    vh_display_semaphore = xSemaphoreCreateMutex();
    vm_sdcard_semaphor = xSemaphoreCreateMutex();
    sensor_status_bits = xEventGroupCreate();
    vh_raw_measurement_queue = xQueueCreate(200, sizeof(raw_measurement_msg_t));
    vh_gps_queue = xQueueCreate(100, sizeof(gps_data_t));
    vh_gps_csv_queue = xQueueCreate(400, sizeof(gps_data_t));
    vm_csv_queue = xQueueCreate(2000, sizeof(raw_measurement_msg_t));

    if (vh_display_semaphore == NULL)
    {
        ESP_LOGE(TAG, "Failed to create display semaphore");
    }

    if (vh_display_semaphore == NULL)
    {
        ESP_LOGE(TAG, "Failed to create display semaphore");
    }

    if (vh_gps_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create vh_gps_queue");
    }

    if (vm_csv_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create vm_csv_queue");
    }

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

#ifdef USE_EPAPER
    display_init();
    xTaskCreatePinnedToCore(
        display_task_code,   /* Function to implement the task */
        "display_task_code", /* Name of the task */
        10 * 1024,           /* Stack size in words */
        NULL,                /* Task input parameter */
        0,                   /* Priority of the task */
        &display_task,       /* Task handle. */
        1);                  /* Core where the task should run */
#else
    xTaskCreatePinnedToCore(
        display_task_code, /* Function to implement the task */
        "display_task",    /* Name of the task */
        32 * 1024,         /* Stack size in words */
        NULL,              /* Task input parameter */
        0,                 /* Priority of the task */
        &display_task,     /* Task handle. */
        1);                /* Core where the task should run */
#endif

    //PSRAM Initialisation
    if (psramInit())
    {
        Serial.println("\nThe PSRAM is correctly initialized");
        Serial.println((String) "PSRAM Size available (bytes): " + ESP.getFreePsram());
    }
    else
    {
        Serial.println("\nPSRAM does not work");
    }

    // ble_sensors.push_back((sensor_definition_t){
    //     .device_name = DEVICE_NAME_HR,
    //     .metric = metric_type_t::HR_BPM,
    //     .service_id = BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
    //     .characteristic_id = {{measurement_t::heartrate, BLEUUID((uint16_t)0x2A37)}},
    //     .address = missing_address,
    //     .client = nullptr,
    //     .parse_data = ble_parse_hr_data,
    //     .enabled = false,
    //     .address_type = esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM,
    //     .has_notification = true,
    // });
    // ble_sensors.push_back((sensor_definition_t){
    //     .device_name = DEVICE_NAME_HR2,
    //     .metric = metric_type_t::HR_BPM,
    //     .service_id = BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
    //     .characteristic_id = {{measurement_t::heartrate, BLEUUID((uint16_t)0x2A37)}},
    //     .address = missing_address,
    //     .client = nullptr,
    //     .parse_data = ble_parse_hr_data,
    //     .enabled = false,
    //     .address_type = esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM,
    //     .has_notification = true,
    // });
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
    // ble_sensors.push_back((sensor_definition_t){
    //     .device_name = DEVICE_NAME_SPEED,
    //     .metric = metric_type_t::SPEED_WHEEL_RPM,
    //     .service_id = BLEUUID("00001816-0000-1000-8000-00805f9b34fb"),
    //     .characteristic_id = {{measurement_t::speed, BLEUUID((uint16_t)0x2A5B)}},
    //     .address = missing_address,
    //     .client = nullptr,
    //     .parse_data = ble_parse_speed_wheel_rpm_data,
    //     .enabled = false,
    //     .address_type = esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM,
    //     .has_notification = true,
    // });

    ble_sensors.push_back((sensor_definition_t){
        .device_name = DEVICE_NAME_AIRSPEED,
        .metric = metric_type_t::AIRSPEED_KMH,
        .service_id = BLEUUID("0895EC4E-F5A8-47AD-BDDE-FDEBB46D6F93"),
        .characteristic_id = airspeed_metrics_def,
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_airspeed,
        .enabled = true,
        .address_type = esp_ble_addr_type_t::BLE_ADDR_TYPE_PUBLIC,
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
#endif
#ifdef FEATURE_GPS_ENABLED
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