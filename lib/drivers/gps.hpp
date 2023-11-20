#ifndef VELOHUB_GPS_HPP
#define VELOHUB_GPS_HPP

#include "config.h"
#include "types.hpp"
#include <tuple>
#include <float.h>
#include <math.h>
#include <TinyGPSPlus.h>

struct pixel_t
{
    uint16_t x = 0;
    uint16_t y = 0;
};

pixel_t convert_geo_to_pixel(float_t latitude, float_t longitude,
                             float_t mapWidth,            // in pixels
                             float_t mapHeight,           // in pixels
                             float_t mapLonLeft,          // in degrees
                             float_t mapLonDelta,         // in degrees (mapLonRight - mapLonLeft);
                             float_t mapLatBottom,        // in degrees
                             float_t mapLatBottomDegree); // in Radians

void gps_task_code(void *parameter);

#endif
