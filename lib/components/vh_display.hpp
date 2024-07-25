#pragma once

#include <lvgl>
#include "esp32-hal-log.h"
#include "types.hpp"

void publish(uint32_t topic, window_counter_t payload);
void publish(uint32_t topic, gps_data_t payload);
void publish(uint32_t topic, raw_measurement_msg_t payload);