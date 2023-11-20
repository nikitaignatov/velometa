#ifndef _VELOHUB_SPEED_HPP_
#define _VELOHUB_SPEED_HPP_

#include "sensor.hpp"

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