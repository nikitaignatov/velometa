#ifndef VELOHUB_METRIC_H
#define VELOHUB_METRIC_H

#include "queue.h"
#include "types.h"
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
    char *name;

public:
    Metric(char *name)
    {
        this->name = name;
        reset();
    }
    void new_reading(float_t value);
    metric_data_t last();
    metric_data_t period(uint16_t seconds);
    void reset();
};

#endif