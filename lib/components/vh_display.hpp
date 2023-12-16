#pragma once

#include <lvgl.h>
#include "esp32-hal-log.h"
#include "types.hpp"

void publish(uint32_t topic, window_counter_t payload);
void publish(uint32_t topic, gps_data_t payload);