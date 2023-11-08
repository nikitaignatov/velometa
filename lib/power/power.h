#ifndef POWER_H
#define POWER_H
#include "sensor.h"
#include "zones.h"

class Power : public Sensor
{
    char const *name;
    long wait = 0;

    void interpret(uint8_t *pData, size_t length) override;

public:
    Power(char const *device_name, size_t buffer_size)
        : Sensor(
              device_name,
              buffer_size,
              BLEUUID("00001818-0000-1000-8000-00805f9b34fb"),
              BLEUUID((uint16_t)0x2A63))
    {
        name = device_name;
    }
};

#endif