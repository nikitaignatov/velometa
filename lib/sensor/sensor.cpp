#include "sensor.h"

void Metric::reset()
{
    last_value = 0;
    min_value = 0;
    max_value = 0;
    avg_value = 0;
    sum_value = 0;
    count_value = 0;
}

void Metric::new_reading(float_t value)
{
    last_value = value;
    if (min_value > value)
        min_value = value;
    if (max_value < value)
        max_value = value;
    sum_value += value;
    count_value++;
    avg_value = sum_value / count_value;
}

float_t Metric::min()
{
    return min_value;
}

float_t Metric::max()
{
    return max_value;
}

float_t Metric::avg()
{
    return avg_value;
}