#include "vh_display.hpp"

void publish(uint32_t topic, metric_info_t payload)
{
    if (xSemaphoreTake(vh_display_semaphore, (TickType_t)50) == pdTRUE)
    {
        lv_msg_send(topic, &payload);
        xSemaphoreGive(vh_display_semaphore);
    }
    else
    {
        ESP_LOGE("metric_info_t", "failed to take semaphor");
    }
}

void publish(uint32_t topic, gps_data_t payload)
{
    if (xSemaphoreTake(vh_display_semaphore, (TickType_t)50) == pdTRUE)
    {
        lv_msg_send(topic, &payload);
        xSemaphoreGive(vh_display_semaphore);
    }
    else
    {
        ESP_LOGE("gps_data_t", "failed to take semaphor");
    }
}
