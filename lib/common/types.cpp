#include "types.hpp"

void publish_measurement(std::optional<float> input, measurement_t type, uint64_t ts)
{
    static const char *TAG = "publish_measurement";
    static raw_measurement_msg_t msg;
    if (input.has_value())
    {
        msg = {
            .ts = ts,
            .measurement = type,
            .value = input.value(),
        };
        if (vm_csv_queue)
        {
            if (xQueueSend(vm_csv_queue, &msg, 10) != pdTRUE)
            {
                ESP_LOGE(TAG, "measurement[%d] [%.2f] was not sent to csv.\n", type, input.value());
            }
        }
        else
        {
            ESP_LOGE(TAG, "vm_csv_queue is not initialized.\n");
        }
        if (vh_raw_measurement_queue)
        {
            xQueueSend(vh_raw_measurement_queue, &msg, 0);
        }
        else
        {

            ESP_LOGE(TAG, "vm_raw_measurement_queue is not initialized.\n");
        }
    }
    else
    {
        ESP_LOGE(TAG, "measurement[%d] is missing value.\n", type);
    }
}
