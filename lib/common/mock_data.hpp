#pragma once

#include <Arduino.h>
#include "config.hpp"
#include "types.hpp"
#include "freertos/event_groups.h"

extern uint16_t map_frequency;
extern void publish(uint32_t topic, metric_info_t payload);
extern void publish(uint32_t topic, gps_data_t payload);

void vh_mock_data_toggle();
void mock_task_code(void *parameter);