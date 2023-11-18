#include "queue.h"

#include <float.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
typedef float float_t;

class Metric
{
    char * name;
    float_t last_value, max_value, min_value, avg_value, sum_value, count_value;

public:
    Metric(char* name)
    {
        this->name = name;
        reset();
    }
    void new_reading(float_t value);
    float_t last();
    float_t max();
    float_t min();
    float_t avg();
    void reset();
};

// class Sensor
// {
//     String name;

// public:
//     Sensor(String name)
//     {
//         this->name = name;
//         reset();
//     }
//     void reset();
// };