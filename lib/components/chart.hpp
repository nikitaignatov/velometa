#pragma once

#include <lvgl.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "activity.hpp"
#include "fonts.hpp"
#include "ui_base.hpp"

class Chart : public UI_Base
{
private:
    lv_obj_t *obj;
    lv_obj_t *label;

    lv_chart_series_t *series_primary;
    lv_chart_series_t *series_secondary;
    void event_cb(lv_event_t *e);

public:
    Chart(const Chart &temp_obj) = delete;
    Chart &operator=(const Chart &temp_obj) = delete;

    ~Chart()
    {
        lv_chart_remove_series(obj, series_primary);
        lv_chart_remove_series(obj, series_secondary);
        lv_obj_del_async(obj);
        lv_obj_del_async(label);
    }

    Chart(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, uint16_t points_pr_series);
    void subscribe(measurement_t msg_id);
    void add_primary_series(measurement_t msg_id, uint32_t color, uint16_t min, uint16_t max);
    void add_secondary_series(measurement_t msg_id, uint32_t color, uint16_t min, uint16_t max);
};