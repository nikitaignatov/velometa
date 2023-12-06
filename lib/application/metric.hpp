#ifndef _VELOHUB_METRIC_HPP_
#define _VELOHUB_METRIC_HPP_

#include <vector>
#include "queue.hpp"
#include "types.hpp"
#include <float.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
typedef float float_t;
void metric_task_code(void *parameter);

typedef struct
{
    size_t count;
    float_t sum;
    uint64_t period_start;
    uint64_t period_end;
    float_t min;
    float_t max;
    float_t avg;
    float_t last;
} metric_data_t;

class Metric
{
    char const *name;
    std::vector<metric_t> measurements;

public:
    Metric(char const *name)
    {
        this->name = name;
        reset();
    }
    void new_reading(metric_t value);
    metric_data_t last();
    metric_data_t period(uint16_t seconds);
    void reset();
};

#endif