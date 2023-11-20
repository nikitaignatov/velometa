#ifndef HR_H
#define HR_H

#include "sensor.hpp"
#include "zones.hpp"

class HR : public Sensor
{
    char const *name;

public:
    HR(char const *device_name, size_t buffer_size)
        : Sensor(
              device_name,
              buffer_size)
    {
        name = device_name;
    }
};

#endif