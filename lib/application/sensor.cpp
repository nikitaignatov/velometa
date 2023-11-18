#include "sensor.h"

uint16_t Sensor::last() { return lastv; }
uint16_t Sensor::avg() { return avgv; }
uint16_t Sensor::min() { return minv; }
uint16_t Sensor::max() { return maxv; }
uint8_t Sensor::zone() { return zonev; }
void Sensor::init()
{
    if (name == "XXX")
    {
        return;
    }
}
