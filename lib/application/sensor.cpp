#include "sensor.h"

uint16_t Sensor::last() { return lastv; }
uint16_t Sensor::avg() { return avgv; }
uint16_t Sensor::min() { return minv; }
uint16_t Sensor::max() { return maxv; }
uint8_t Sensor::zone() { return zonev; }

void Sensor::init() {}

void Sensor::add_reading(uint16_t value)
{
    lastv = value;
    sumv += value;
    count++;
    avgv = sumv / count;
    if (minv > value)
        minv = value;
    if (maxv < value)
        maxv = value;
    enqueue(queue, value);
}