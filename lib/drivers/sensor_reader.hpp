#pragma once

#include <fmt/core.h>
#include <fmt/ranges.h>
#include "config.hpp"
#include "types.hpp"
#include "environmental_sensor.hpp"
#include "position_sensor.hpp"
#include "differential_pressure_sensor.hpp"

extern uint64_t ts();
void sensor_reader_task_code(void *parameter);
