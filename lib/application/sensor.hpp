#ifndef VELOHUB_SENSOR_H
#define VELOHUB_SENSOR_H

#include "queue.h"

class Sensor
{
    char const *name;

protected:
    uint16_t minv, maxv, avgv, lastv, sumv, count, zonev, lastr;

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
    uint8_t zone();
};

#endif