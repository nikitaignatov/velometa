#pragma once

#include "config.hpp"
#include "types.hpp"
#include <tuple>
#include <float.h>
#include "I2Cdev.h"
#include <math.h>
#include "Wire.h"
#include <optional>
#include <SparkFun_VL53L5CX_Library.h>

class DistanceSensor
{
private:
    SparkFun_VL53L5CX myImager;
    uint8_t _address;
    std::optional<float> _roll, _pitch, _yaw;
    bool ready = false;

    int imageResolution = 0; // Used to pretty print output
    int imageWidth = 0;      // Used to pretty print output

public:
    bool read_sensor();
    VL53L5CX_ResultsData measurementData;

    void init();
};
