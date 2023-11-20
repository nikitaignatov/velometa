#include <Arduino.h>
#include "types.h"
#include "ble.h"
#include "gps.h"
#include "display_420.h"

TaskHandle_t ble_task;
TaskHandle_t display_task;
TaskHandle_t sensor_task;
TaskHandle_t gps_task;

std::vector<sensor_definition_t> ble_sensors;
QueueHandle_t vh_raw_measurement_queue;
QueueHandle_t vh_metrics_queue;

HR hr_monitor(DEVICE_NAME_HR, 400);
Power power_monitor(DEVICE_NAME_POWER, 400);
Speed speed_monitor(DEVICE_NAME_SPEED, 10);

void display_task_code(void *parameter)
{
    Serial.println("display_task_code");
    long last = 0;
    uint8_t refresh = 0;
    for (;;)
    {
        long secs = millis();
        if (refresh)
        {
            refresh_screen();
            refresh = false;
        }
        render(secs / 1000, &hr_monitor, &power_monitor, &speed_monitor);
        // display_bottom(height, speed, lat, lon);
        show();
        delay(1000);
    }
}

void sensor_task_code(void *parameter)
{
    Serial.println("sensor_task_code");
    raw_measurement_msg_t msg;
    for (;;)
    {
        if (xQueueReceive(vh_raw_measurement_queue, &msg, 1000 / portTICK_RATE_MS) == pdPASS)
        {
            switch (msg.measurement)
            {
            case measurement_t::heartrate:
                hr_monitor.add_reading(msg.value);
                break;
            case measurement_t::power:
                power_monitor.add_reading(msg.value);
                break;
            case measurement_t::speed:
                speed_monitor.add_reading(msg.value / msg.scale);
                Serial.printf("SPEED\t: %.2f km/h\n", msg.value / (float_t)msg.scale);
                break;
            case measurement_t::elevation:
                Serial.printf("ELEVATION\t: %d meters\n", msg.value / msg.scale);
                break;

            default:
                Serial.printf("Unhandled Message type: %i, Value: %i\n", msg.measurement, msg.value);
                break;
            }
        }
        else
        {
            Serial.println("No items on vh_raw_measurement_queue.");
        }
    }
}

void setup()
{
    Serial.begin(115200);

    vh_raw_measurement_queue = xQueueCreate(100, sizeof(raw_measurement_msg_t));
    ble_sensors.push_back((sensor_definition_t){
        {.device_name = DEVICE_NAME_HR},
        .metric = metric_type_t::HR_BPM,
        .service_id = BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = BLEUUID((uint16_t)0x2A37),
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_hr_data,
        .enabled = true,
    });
    ble_sensors.push_back((sensor_definition_t){
        {.device_name = DEVICE_NAME_HR2},
        .metric = metric_type_t::HR_BPM,
        .service_id = BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = BLEUUID((uint16_t)0x2A37),
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_hr_data,
        .enabled = true,
    });
    ble_sensors.push_back((sensor_definition_t){
        {.device_name = DEVICE_NAME_POWER},
        .metric = metric_type_t::POWER_WATT,
        .service_id = BLEUUID("00001818-0000-1000-8000-00805f9b34fb"),
        .characteristic_id = BLEUUID((uint16_t)0x2A63),
        .address = missing_address,
        .client = nullptr,
        .parse_data = ble_parse_power_watt_data,
        .enabled = true,
    });
    // ble_sensors.push_back((sensor_definition_t){
    //     {.device_name = DEVICE_NAME_SPEED},
    //     .metric = metric_type_t::SPEED_WHEEL_RPM,
    //     .service_id = BLEUUID("00001816-0000-1000-8000-00805f9b34fb"),
    //     .characteristic_id = BLEUUID((uint16_t)0x2A5B),
    //     .address = missing_address,
    //     .client = nullptr,
    //     .parse_data = ble_parse_speed_wheel_rpm_data,
    //     .enabled = true,
    // });

#ifdef FEATURE_SCREEN_ENABLED
    display_init();
    xTaskCreatePinnedToCore(
        display_task_code,   /* Function to implement the task */
        "display_task_code", /* Name of the task */
        10 * 1024,           /* Stack size in words */
        NULL,                /* Task input parameter */
        0,                   /* Priority of the task */
        &display_task,       /* Task handle. */
        1);                  /* Core where the task should run */
#endif

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
        0);              /* Core where the task should run */
#endif

    vTaskDelete(NULL);
}

void loop()
{
}