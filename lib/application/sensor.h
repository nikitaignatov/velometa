#ifndef VELOHUB_SENSOR_H
#define VELOHUB_SENSOR_H

#include <Arduino.h>
#include <BLEDevice.h>
#include "queue.h"
#include <queue.h>

struct pinRead
{
    int pin;
    int value;
};

class Sensor
{
    QueueHandle_t structQueue;
    char const *name;

protected:
    uint16_t minv, maxv, avgv, lastv, sumv, count, zonev, lastr;

public:
    Sensor(char const *device_name, size_t buffer_size)
    {
        this->name = device_name;
        this->queue = createQueue(buffer_size);
        structQueue = xQueueCreate(10, sizeof(struct pinRead));
    }
    virtual ~Sensor()
    {
    }
    struct Queue *queue;
    void init();
    uint16_t last();
    uint16_t min();
    uint16_t max();
    uint16_t avg();
    uint8_t zone();
};

#endif