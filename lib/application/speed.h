#ifndef VELOHUB_SPEED_H
#define VELOHUB_SPEED_H

#include "sensor.h"

class Speed : public Sensor
{
    char const *name;

public:
    Speed(char const *device_name, size_t buffer_size)
        : Sensor(
              device_name,
              buffer_size)
    {
        name = device_name;
    }
};

#endif