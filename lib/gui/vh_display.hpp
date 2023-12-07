
#pragma once

#include <lvgl.h>
#include "types.hpp"

void publish(uint32_t topic, metric_info_t payload);
void publish(uint32_t topic, gps_data_t payload);