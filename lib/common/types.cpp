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