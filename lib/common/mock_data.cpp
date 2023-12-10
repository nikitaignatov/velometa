#include "mock_data.hpp"

void vh_mock_data_toggle()
{
    auto bits = xEventGroupGetBits(sensor_status_bits);
    auto bit_is_set = ((bits & VH_SENSOR_BIT_MOCK_DATA) != 0);
    if (bit_is_set)
    {
        xEventGroupClearBits(sensor_status_bits, VH_SENSOR_BIT_MOCK_DATA);
    }
    else
    {
        xEventGroupSetBits(sensor_status_bits, VH_SENSOR_BIT_MOCK_DATA);
    }
}

void mock_task_code(void *parameter)
{
    Serial.println("mock_task_code");
    metric_info_t msg = {};

    metric_t m;
    gps_data_t g;
    metric_info_t s = {};
    metric_info_t h = {};
    metric_info_t p = {};
    metric_info_t n = {};
    for (;;)
    {
        vTaskDelay(map_frequency / portTICK_PERIOD_MS);
        xEventGroupWaitBits(sensor_status_bits, VH_SENSOR_BIT_MOCK_DATA, pdFALSE, pdTRUE, portMAX_DELAY);

        m = {
            .ts = 0,
            .value = (float)random(15, 60) / (float)1,
        };
        s = update_stats(s, m);
        publish(MSG_NEW_SPEED, s);
        m = {
            .ts = 0,
            .value = (float)random(110, 200) / (float)1,
        };
        h = update_stats(h, m);
        publish(MSG_NEW_HR, h);
        m = {
            .ts = 0,
            .value = (float)random(10, 600) / (float)1,
        };
        p = update_stats(p, m);
        publish(MSG_NEW_POWER, p);
        g = {
            .tick_ms = (uint64_t)random(90, 300),
            .date = (uint64_t)random(90, 300),
            .time = (uint64_t)random(90, 300),
            .lat = (double)random(50000, 55999) / 1000.0,
            .lon = (double)random(10000, 12999) / 1000.0,
            .speed = (double)random(500, 5599) / 100.0,
            .height = (double)random(30, 30000) / 100.0,
            .hdop = (double)random(1, 200) / 10.0,
            .age = (double)random(5000, 55999) / 1000.0,
            .has_fix = (bool)random(0, 2),
            .satelites = (uint32_t)random(0, 9),
            .mocked = true,
        };
        publish(MSG_NEW_GPS, g);
    }
}