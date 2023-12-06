#ifndef _VELOHUB_SENSOR_HPP_
#define _VELOHUB_SENSOR_HPP_

#include "queue.hpp"
#include <Arduino.h>
#include "config.hpp"
#include "types.hpp"
#include "metric.hpp"
#include "vh_display.hpp"

class Sensor
{
    char const *name;

protected:
    uint16_t minv, maxv, avgv, lastv, sumv, count, lastr;

public:
    Sensor(char const *device_name, size_t buffer_size)
    {
        this->name = device_name;
        this->queue = createQueue(buffer_size);
    }
    virtual ~Sensor()
    {
    }
    struct Queue *queue;
    void init();
    void add_reading(uint16_t value);
    uint16_t last();
    uint16_t min();
    uint16_t max();
    uint16_t avg();
};

extern Metric hr_metric;

void sensor_task_code(void *parameter);
#endif