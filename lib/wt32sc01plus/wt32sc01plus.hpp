#pragma once

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <float.h>

#include "sdcard.hpp"
#include "config.hpp"

// #define LGFX_AUTODETECT // Autodetect board
#define LV_CONF_INCLUDE_SIMPLE
#define LV_USE_THEME_NIGHT 1
/* Uncomment below line to draw on screen with touch */

#include "lgfx.hpp"


#include <lvgl.h>
#include "lv_conf.h"

#include "types.hpp"
#include "vh_tiles.hpp"
#include "vh_map.hpp"
#include "vh_status_bar.hpp"
#include "vh_theme.hpp"


extern uint16_t map_frequency;
extern system_t sys;

void display_task_code(void *parameter);