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
    uint8_t _address;
    Adafruit_BME280 sensor;
    std::optional<float> _pressure,_temperature,_humidity;

public:
    bool read_sensor();
    std::optional<float> get_air_pressure();
    std::optional<float> get_air_temperature();
    std::optional<float> get_air_relative_humidity();
    std::optional<float> get_air_density();
    std::optional<float> get_elevation();
    void init(uint8_t address);
};
