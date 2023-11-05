#ifndef POWER_H
#define POWER_H
#include <Arduino.h>
#include "zones.h"
#include "sensor.h"


class Power : public Sensor
{
    String name;
    long wait = 0;
    // BLE
    boolean newHr = false;

public:
    Power(String device_name, size_t buffer_size)
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