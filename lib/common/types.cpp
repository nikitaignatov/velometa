#include "types.hpp"
void publish_measurement(std::optional<float> input, measurement_t type, uint64_t ts)
{
    static const char *TAG = "publish_measurement";
    static raw_measurement_msg_t msg;
    if (input.has_value())
    {
        msg = {
            .measurement = type,
            .ts = ts,
            .value = input.value(),
        };
        if (vh_raw_measurement_queue)
        {
            if (xQueueSend(vh_raw_measurement_queue, &msg, 0) != pdTRUE)
            {
                ESP_LOGE(TAG, "measurement[%d] [%.2f] was not sent.\n", type, input.value());
            }
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
