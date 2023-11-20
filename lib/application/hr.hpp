#ifndef _HR_HPP_
#define _HR_HPP_

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