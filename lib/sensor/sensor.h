#include <Arduino.h>
#include "queue.h"

class Metric
{
    String name;
    float_t last_value, max_value, min_value, avg_value, sum_value, count_value;

public:
    Metric(String name)
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