#pragma once

#include <lvgl.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "vh_container.hpp"
#include "vh_gps.hpp"
#include "vh_map.hpp"
#include "vh_screen_brightness.hpp"
#include "vh_dashboard.hpp"
#include "vh_mock_btn.hpp"
#include "vh_screen_activity_hr_stats.hpp"
#include "vh_histogram.hpp"


lv_obj_t *vh_tiles_init(lv_obj_t *parent);
lv_obj_t *vh_get_main_tile();
lv_obj_t *vh_get_settings_tile();
lv_obj_t *vh_get_map_tile();