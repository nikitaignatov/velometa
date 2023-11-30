#ifndef _VH_WT32SC01_HPP_
#define _VH_WT32SC01_HPP_

#include "sdcard.hpp"
#include "config.hpp"

extern uint16_t map_frequency;
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <float.h>

#include "types.hpp"

extern system_t sys;
void vh_setup();
void vh_loop();
void update(system_t sys);
void publish(uint32_t topic, metric_info_t payload);

#endif