#pragma once

#include <lvgl.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "vh_container.hpp"

lv_obj_t *vh_tiles_init(lv_obj_t *parent);
lv_obj_t *vh_get_main_tile();
lv_obj_t *vh_get_settings_tile();
lv_obj_t *vh_get_map_tile();