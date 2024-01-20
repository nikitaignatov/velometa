#pragma once

#include <lvgl.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "vh_container.hpp"
#include "activity.hpp"
#include "vh_fonts.hpp"

void vh_create_histogram(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, uint16_t points_pr_series);