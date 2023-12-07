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
    metric_info_t s = {};
    metric_info_t h = {};
    metric_info_t p = {};
    metric_info_t n = {};
    for (;;)
    {
        xEventGroupWaitBits(sensor_status_bits, VH_SENSOR_BIT_MOCK_DATA, pdFALSE, pdTRUE, portMAX_DELAY);
        vTaskDelay(map_frequency / portTICK_PERIOD_MS);

        m = {
            .ts = 0,
            .value = (float)random(15, 60) / (float)1,
        };
        s = update_stats(s, m);
        publish(MSG_NEW_SPEED, s);
        m = {
            .ts = 0,
            .value = (float)random(45, 200) / (float)1,
        };
        h = update_stats(h, m);
        publish(MSG_NEW_HR, h);
        m = {
            .ts = 0,
            .value = (float)random(90, 300) / (float)1,
        };
        p = update_stats(p, m);
        publish(MSG_NEW_POWER, p);
        publish(MSG_NEW_HEADING, msg);
    }
}