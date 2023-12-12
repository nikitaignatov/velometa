
#include "config.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <Arduino.h>
#include "types.hpp"
#include "ble.hpp"
#include "sensor.hpp"
#include "gps.hpp"
#include "mock_data.hpp"
#include "computer.hpp"

uint16_t map_frequency = 500;
SemaphoreHandle_t vh_display_semaphore;

#if USE_EPAPER
#include "display_420.hpp"
#elif USE_LCD
#include "wt32sc01plus.hpp"
#endif

TaskHandle_t ble_task;
TaskHandle_t display_task;
TaskHandle_t sensor_task;
TaskHandle_t gps_task;

std::vector<sensor_definition_t> ble_sensors;
QueueHandle_t vh_raw_measurement_queue;
QueueHandle_t vh_metrics_queue;
QueueHandle_t vh_gps_queue;
EventGroupHandle_t sensor_status_bits;
Ride ride;
void setup()
{
    Serial.begin(115200);
    Serial.println("VeloHUB");

    vh_display_semaphore = xSemaphoreCreateMutex();
    if (vh_display_semaphore == NULL)
    {
        Serial.println("Failed to create display semaphore");
    }

#ifdef DEVELOPMENT
    delay(4000);
#endif
    //PSRAM Initialisation
    if (psramInit())
    {
        Serial.println("\nThe PSRAM is correctly initialized");
    }
    else
    {
        Serial.println("\nPSRAM does not work");
    }

    int available_PSRAM_size = ESP.getFreePsram();
    Serial.println((String) "PSRAM Size available (bytes): " + available_PSRAM_size);
    ride.init();
    Serial.println((String) "PSRAM Size available (bytes): " + ESP.getFreePsram());

    sensor_status_bits = xEventGroupCreate();

    vh_raw_measurement_queue = xQueueCreate(100, sizeof(raw_measurement_msg_t));
    vh_gps_queue = xQueueCreate(120, sizeof(gps_data_t));

    ble_sensors.push_back((sensor_definition_t){
        .device_name = DEVICE_NAME_HR,
        .metric = metric_type_t::HR_BPM,
        .service_id = BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = BLEUUID((uint16_t)0x2A37),
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_hr_data,
        .enabled = true,
    });
    ble_sensors.push_back((sensor_definition_t){
        .device_name = DEVICE_NAME_HR2,
        .metric = metric_type_t::HR_BPM,
        .service_id = BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = BLEUUID((uint16_t)0x2A37),
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_hr_data,
        .enabled = true,
    });
    ble_sensors.push_back((sensor_definition_t){
        .device_name = DEVICE_NAME_POWER,
        .metric = metric_type_t::POWER_WATT,
        .service_id = BLEUUID("00001818-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = BLEUUID((uint16_t)0x2A63),
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_power_watt_data,
        .enabled = true,
    });
    ble_sensors.push_back((sensor_definition_t){
        .device_name = DEVICE_NAME_SPEED,
        .metric = metric_type_t::SPEED_WHEEL_RPM,
        .service_id = BLEUUID("00001816-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = BLEUUID((uint16_t)0x2A5B),
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_speed_wheel_rpm_data,
        .enabled = true,
    });

    auto r = fs_mount_sd_card();
    Serial.print(r->fsize);
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
        24 * 1024,         /* Stack size in words */
        NULL,              /* Task input parameter */
        0,                 /* Priority of the task */
        &display_task,     /* Task handle. */
        1);                /* Core where the task should run */
#endif

    xTaskCreatePinnedToCore(
        mock_task_code, /* Function to implement the task */
        "mock_task",    /* Name of the task */
        8 * 1024,       /* Stack size in words */
        NULL,           /* Task input parameter */
        0,              /* Priority of the task */
        NULL,           /* Task handle. */
        1);             /* Core where the task should run */
    xTaskCreatePinnedToCore(
        write_task_code, /* Function to implement the task */
        "write_task",    /* Name of the task */
        4 * 1024,        /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        NULL,            /* Task handle. */
        1);              /* Core where the task should run */

    xTaskCreatePinnedToCore(
        ble_task_code,   /* Function to implement the task */
        "ble_task_code", /* Name of the task */
        16 * 1024,       /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &ble_task,       /* Task handle. */
        0);              /* Core where the task should run */

    xTaskCreatePinnedToCore(
        sensor_task_code,   /* Function to implement the task */
        "sensor_task_code", /* Name of the task */
        16 * 1024,          /* Stack size in words */
        NULL,               /* Task input parameter */
        0,                  /* Priority of the task */
        &sensor_task,       /* Task handle. */
        1);                 /* Core where the task should run */
#ifdef FEATURE_GPS_ENABLED
    xTaskCreatePinnedToCore(
        gps_task_code,   /* Function to implement the task */
        "gps_task_code", /* Name of the task */
        4 * 1024,        /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &gps_task,       /* Task handle. */
        1);              /* Core where the task should run */
#endif
#ifdef FEATURE_GPS_ENABLED
    xTaskCreatePinnedToCore(
        gps_process_task_code,   /* Function to implement the task */
        "gps_process_task_code", /* Name of the task */
        4 * 1024,                /* Stack size in words */
        NULL,                    /* Task input parameter */
        0,                       /* Priority of the task */
        &gps_task,               /* Task handle. */
        1);                      /* Core where the task should run */
#endif

    vTaskDelete(NULL);
}

void loop()
{
}