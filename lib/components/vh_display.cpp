#include "vh_display.hpp"

void publish(uint32_t topic, window_counter_t payload)
{
    const unsigned MEASUREMENTS = 1;
    uint64_t start = esp_timer_get_time();

    for (int retries = 0; retries < MEASUREMENTS; retries++)
    {
        if (xSemaphoreTake(vh_display_semaphore, (TickType_t)2000) == pdTRUE)
        {
            lv_msg_send(topic, &payload);
            xSemaphoreGive(vh_display_semaphore);
        }
        else
        {
            ESP_LOGE("metric_info_t", "failed to take semaphor %d",topic);
        }
    }
    uint64_t end = esp_timer_get_time();

    printf("%u iterations took %llu milliseconds (%llu microseconds per invocation)\n",
           MEASUREMENTS, (end - start) / 1000, (end - start) / MEASUREMENTS);
}

void publish(uint32_t topic, gps_data_t payload)
{
    if (xSemaphoreTake(vh_display_semaphore, (TickType_t)200) == pdTRUE)
    {
        lv_msg_send(topic, &payload);
        xSemaphoreGive(vh_display_semaphore);
    }
    else
    {
        ESP_LOGD("gps_data_t", "failed to take semaphor");
    }
}


void publish(uint32_t topic, raw_measurement_msg_t payload)
{
    if (xSemaphoreTake(vh_display_semaphore, (TickType_t)200) == pdTRUE)
    {
        lv_msg_send(topic, &payload);
        xSemaphoreGive(vh_display_semaphore);
    }
    else
    {
        ESP_LOGD("gps_data_t", "failed to take semaphor");
    }
}
