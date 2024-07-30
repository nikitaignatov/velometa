#pragma once

#include <fmt/core.h>
#include <fmt/ranges.h>
#include "config.hpp"
#include "types.hpp"
#include <tuple>
#include <float.h>
#include <math.h>
#include "Wire.h"
#include "Adafruit_VL53L0X.h"

class PositionTrackingSensor
{
private:
    Adafruit_VL53L0X sensor;

    float y = 0.0f;
    float a = 0.80f;
    bool debug = false;
    bool ready = false;
    std::optional<float> _distance;

public:
    bool read_sensor();
    std::optional<float> get_distance_mm();
    void init(uint8_t address);
};
