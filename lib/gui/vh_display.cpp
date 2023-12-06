#include "vh_display.hpp"

void publish(uint32_t topic, metric_info_t payload)
{
    lv_msg_send(topic, &payload);
}

void publish(uint32_t topic, gps_data_t payload)
{
    lv_msg_send(topic, &payload);
}
