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
    lv_obj_t *value;

    lv_chart_series_t *ser_hr_avg;
    lv_chart_series_t *ser_hr_last;
    lv_chart_series_t *ser_p_avg;
    lv_chart_series_t *ser_p_last;
    lv_chart_cursor_t *cursor;
    void event_cb(lv_event_t *e);

public:
    Chart(const Chart &temp_obj) = delete;
    Chart &operator=(const Chart &temp_obj) = delete;

    ~Chart()
    {
        lv_obj_del_async(obj);
        lv_obj_del_async(label);
        lv_obj_del_async(value);
    }

    Chart(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, uint16_t points_pr_series);
    void subscribe(measurement_t msg_id);
};