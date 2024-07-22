#pragma once

#include "config.hpp"
#include "types.hpp"
#include <tuple>
#include <float.h>
#include <math.h>
#include "Wire.h"
#include <optional>
#include <XGZP6897D.h>
#include <SimpleKalmanFilter.h>

#define SENSITIVITY_FACTOR 4096

class DifferentialPressureSensor
{
private:
    SimpleKalmanFilter kf = SimpleKalmanFilter(1, 1, 0.1);
    uint8_t address = 0;
    bool ready = false;
    float _pressure_raw, _temperature_raw, _offset;
    std::optional<float> _pressure, _temperature;
    XGZP6897D sensor = XGZP6897D(SENSITIVITY_FACTOR);

public:
    std::optional<float> get_pressure();
    std::optional<float> get_temperature();
    bool read_sensor();
    void init(uint8_t address);
    void change_port(uint8_t port);
};
