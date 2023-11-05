#ifndef VELOHUB_SPEED_H
#define VELOHUB_SPEED_H

#include "queue.h"
#include "sensor.h"


class Speed : public Sensor
{
    char* name;
    int  last_tv, last_rv;
    long wait = 0;

public:
    Speed(char* device_name, size_t buffer_size)
        : Sensor(
              device_name,
              buffer_size,
              BLEUUID("00001816-0000-1000-8000-00805f9b34fb"),
              BLEUUID((uint16_t)0x2A5B))
    {
        name = device_name;
    }

    void interpret(uint8_t *pData, size_t length);
};

#endif