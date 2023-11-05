#ifndef HR_H
#define HR_H

#include "sensor.h"
#include "zones.h"

class HR : public Sensor
{
    char* name;
    long wait = 0;
    // BLE

public:
    HR(char* device_name, size_t buffer_size)
        : Sensor(
              device_name,
              buffer_size,
              BLEUUID("0000180d-0000-1000-8000-00805f9b34fb"),
              BLEUUID((uint16_t)0x2A37))
    {
        name = device_name;
    }

};

#endif