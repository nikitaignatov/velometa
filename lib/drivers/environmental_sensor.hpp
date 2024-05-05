#pragma once

#include "config.hpp"
#include "types.hpp"
#include <tuple>
#include <float.h>
#include <math.h>
#include "Wire.h"
#include <Adafruit_BME280.h>
#include <optional>

class EnvironmentalSensor
{
private:
    Adafruit_BME280 sensor;

public:
    std::optional<float> get_air_pressure();
    std::optional<float> get_air_temperature();
    std::optional<float> get_air_relative_humidity();
    void init(uint8_t address);
};
