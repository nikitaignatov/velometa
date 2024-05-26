#pragma once
#define LV_USE_THEME_NIGHT 1

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <float.h>

#include "lgfx.hpp"
#include "config.hpp"
#include "types.hpp"
#include "sdcard.hpp"
#include <lvgl.h>
#include "lv_conf.h"

#include "theme.hpp"
#include "screen.hpp"

void display_task_code(void *parameter);