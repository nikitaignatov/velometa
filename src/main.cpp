#include <Arduino.h>
#include "types.h"
#include "ble.h"
#include "display_420.h"

TaskHandle_t ble_task;
TaskHandle_t display_task;

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
        0);                  /* Core where the task should run */
#endif

    xTaskCreatePinnedToCore(
        ble_task_code,   /* Function to implement the task */
        "ble_task_code", /* Name of the task */
        16 * 1024,       /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &ble_task,       /* Task handle. */
        0);              /* Core where the task should run */
}

void loop()
{
    raw_measurement_msg_t msg;

    if (xQueueReceive(vh_raw_measurement_queue, &msg, 1000 / portTICK_RATE_MS) == pdPASS)
    {
        switch (msg.measurement)
        {
        case measurement_t::heartrate:
            Serial.printf("HR: %d %llu\n", msg.value, msg.ts / 1000);
            hr_monitor.add_reading(msg.value);
            break;
        case measurement_t::power:
            Serial.printf("POWER: %d %llu\n", msg.value, msg.ts / 1000);
            power_monitor.add_reading(msg.value);
            break;
        case measurement_t::speed:
            Serial.printf("SPEED: %d\n", msg.value / 100.0);
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