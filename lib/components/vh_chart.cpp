#include "vh_chart.hpp"
#include <random>

static lv_obj_t *chart_;
static lv_chart_series_t *ser;
static lv_chart_series_t *ser_;
static lv_chart_series_t *ser__;
static lv_chart_series_t *ser2;
static lv_chart_cursor_t *cursor;

std::default_random_engine _gen;
static uint16_t _random(int a, int b)
{
    std::uniform_int_distribution<uint16_t> distr(a, b);
    return distr(_gen);
}

static void update_event_cb(lv_event_t *e)
{
    auto activity = current_activity();
    auto hr = activity->get_hr(5);
    auto power = activity->get_power(5);
    if (hr.count > 0)
    {
        lv_chart_set_next_value(chart_, ser_, hr.get_last());
        lv_chart_set_next_value(chart_, ser, hr.get_avg());
        lv_chart_set_range(chart_, LV_CHART_AXIS_PRIMARY_Y, hr.get_min() - 2, hr.get_max() + 2);
    }
    if (power.count > 0)
    {
        lv_chart_set_next_value(chart_, ser2, power.get_last());
        lv_chart_set_next_value(chart_, ser__, power.get_avg_w());
        lv_chart_set_range(chart_, LV_CHART_AXIS_SECONDARY_Y, power.get_min(), power.get_max());
    }
}

void vh_create_chart(lv_obj_t *parent)
{
    chart_ = lv_chart_create(parent);
    lv_obj_set_size(chart_, 320, 200);
    lv_obj_align(chart_, LV_ALIGN_CENTER, 0, 0);

    lv_chart_set_range(chart_, LV_CHART_AXIS_PRIMARY_X, 0, 100);
    lv_chart_set_range(chart_, LV_CHART_AXIS_SECONDARY_Y, 100, 800);
    lv_chart_set_point_count(chart_, 120);
    lv_chart_set_axis_tick(chart_, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 6, 5, true, 40);

    lv_obj_set_style_size(chart_, 0, LV_PART_INDICATOR);
    lv_obj_refresh_ext_draw_size(chart_);
    lv_chart_set_div_line_count(chart_, 8, 10);
    lv_chart_set_div_line_count(chart_, 0, 0);

    ser = lv_chart_add_series(chart_, lv_color_hex(0xff0000), LV_CHART_AXIS_PRIMARY_Y);
    ser_ = lv_chart_add_series(chart_, lv_color_hex(0x990000), LV_CHART_AXIS_PRIMARY_Y);
    ser__ = lv_chart_add_series(chart_, lv_color_hex(0x2222ee), LV_CHART_AXIS_SECONDARY_Y);
    ser2 = lv_chart_add_series(chart_, lv_color_hex(0x0000aa), LV_CHART_AXIS_SECONDARY_Y);

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_recolor(label, true);
    lv_label_set_text(label, "#ff0000 HR# #990000 HRavg# & #2222ee Power # #0000aa PWavg #");
    lv_obj_align_to(label, chart_, LV_ALIGN_OUT_TOP_MID, 0, -5);
    lv_obj_add_event_cb(chart_, update_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(MSG_NEW_HR, chart_, 0);
}
