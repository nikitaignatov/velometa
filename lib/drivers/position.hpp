#pragma once

#include <fmt/core.h>
#include <fmt/ranges.h>
#include "config.hpp"
#include "types.hpp"
#include "vh_display.hpp" // TODO: this should not be here
#include <tuple>
#include <float.h>
#include <math.h>
#include "Wire.h"
#include "Adafruit_VL53L0X.h"

extern uint64_t ts();
void position_task_code(void *parameter);
