#include "types.hpp"
#include <Arduino.h>

metric_info_t update_stats(metric_info_t p, metric_t m)
{
    return (metric_info_t){
        .ts = millis(),
        .last = (float)m.value,
        .min = (float)m.value < p.min ? m.value : p.min,
        .max = (float)m.value > p.max ? m.value : p.max,
        .avg = (float)(p.sum + m.value) / (float)p.count + 1,
        .sum = (float)(p.sum + m.value),
        .count = (float)p.count + 1,
        .std = 0,
        .var = 0,
    };
}

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