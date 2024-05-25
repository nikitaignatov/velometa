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

std::default_random_engine gen;
static float random(int a, int b)
{
    std::uniform_int_distribution<uint16_t> distr(a, b);
    return distr(gen);
}

void mock_task_code(void *parameter)
{
    ESP_LOGI("mock", "mock_task_code");

    gps_data_t g;
    raw_measurement_msg_t msg;
    for (;;)
    {
        vTaskDelay(map_frequency / portTICK_PERIOD_MS);
        xEventGroupWaitBits(sensor_status_bits, VH_SENSOR_BIT_MOCK_DATA, pdFALSE, pdTRUE, portMAX_DELAY);

        msg = (raw_measurement_msg_t){
            .ts = 1,
            .measurement = measurement_t::power,
            .value = random(150, 180),
        };
        xQueueSend(vh_raw_measurement_queue, &msg, 50 / portTICK_RATE_MS);
        msg = (raw_measurement_msg_t){
            .ts = 1,
            .measurement = measurement_t::heartrate,
            .value = random(110, 200),
        };
        xQueueSend(vh_raw_measurement_queue, &msg, 50 / portTICK_RATE_MS);
        msg = (raw_measurement_msg_t){
            .ts = 1,
            .measurement = measurement_t::speed,
            .value = random(15, 60),
        };
        xQueueSend(vh_raw_measurement_queue, &msg, 50 / portTICK_RATE_MS);
        g = {
            .tick_ms = (uint64_t)random(90, 300),
            .lat = (float)random(55769, 55788) / 1000.0,
            .lon = (float)random(12151, 12195) / 1000.0,
            .speed = (float)random(500, 5599) / 100.0,
            .height = (uint32_t)random(30, 30000) / 100.0,
            .age = (uint16_t)random(5000, 55999) / 1000.0,
            .has_fix = (bool)random(0, 2),
            .satelites = (uint32_t)random(0, 9),
            .mocked = true,
        };
        publish(MSG_NEW_GPS, g);
    }
}