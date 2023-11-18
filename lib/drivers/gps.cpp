#include "gps.h"

/*
var mapWidth = 1500;
var mapHeight = 1577;

var mapLonLeft = 9.8;
var mapLonRight = 10.2;
var mapLonDelta = mapLonRight - mapLonLeft;

var mapLatBottom = 53.45;
var mapLatBottomDegree = mapLatBottom * Math.PI / 180;
*/

pixel_t convert_geo_to_pixel(float_t latitude, float_t longitude,
                          float_t mapWidth,           // in pixels
                          float_t mapHeight,          // in pixels
                          float_t mapLonLeft,         // in degrees
                          float_t mapLonDelta,        // in degrees (mapLonRight - mapLonLeft);
                          float_t mapLatBottom,       // in degrees
                          float_t mapLatBottomDegree) // in Radians
{
    pixel_t output;
    output.x = (longitude - mapLonLeft) * (mapWidth / mapLonDelta);

    latitude = latitude * M_PI / 180;
    float_t worldMapWidth = ((mapWidth / mapLonDelta) * 360) / (2 * M_PI);
    float_t mapOffsetY = (worldMapWidth / 2 * log((1 + sin(mapLatBottomDegree)) / (1 - sin(mapLatBottomDegree))));
    output.y = mapHeight - ((worldMapWidth / 2 * log((1 + sin(latitude)) / (1 - sin(latitude)))) - mapOffsetY);
    return output;
}