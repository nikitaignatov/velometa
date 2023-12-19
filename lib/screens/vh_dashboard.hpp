
#pragma once

#include <lvgl.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <float.h>

#include "config.hpp"
#include "types.hpp"
#include "vh_container.hpp"
#include "vh_fonts.hpp"
#include "lgfx.hpp"
#include "vh_metric.hpp"
#include "vh_chart.hpp"

extern LGFX lcd;

void vh_create_dashboard(lv_obj_t *parent);