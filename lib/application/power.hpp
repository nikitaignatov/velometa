#ifndef POWER_H
#define POWER_H
#include "sensor.hpp"
#include "zones.hpp"

class Power : public Sensor
{
    char const *name;

public:
    Power(char const *device_name, size_t buffer_size)
        : Sensor(
              device_name,
              buffer_size)
    {
        name = device_name;
    }
};

#endif