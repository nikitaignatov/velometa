#include "vh_chart.hpp"

static lv_obj_t *chart_;

static lv_chart_series_t *ser_hr_avg;
static lv_chart_series_t *ser_hr_last;
static lv_chart_series_t *ser_p_avg;
static lv_chart_series_t *ser_p_last;
static lv_chart_cursor_t *cursor;

static void update_event_cb(lv_event_t *e)
{
    lv_chart_t *chart = (lv_chart_t *)chart_;
    uint32_t offset = 2;

    auto activity = current_activity();
    auto hr = activity->get_hr(3);
    auto hrm = activity->get_hr(8);
    auto power = activity->get_power(2);
    auto powerm = activity->get_power(9);
    if (hr.count > 0)
    {
        lv_chart_set_next_value(chart_, ser_hr_last, hr.get_last());
        lv_chart_set_next_value(chart_, ser_hr_avg, hr.get_avg());

        if (chart->ymin[0] > hrm.get_min() - offset || chart->ymax[0] < hrm.get_max() + offset)
        {
            lv_chart_set_range(chart_, LV_CHART_AXIS_PRIMARY_Y, hrm.get_min() - offset, hrm.get_max() + offset);
        }
    }
    if (power.count > 0)
    {
        lv_chart_set_next_value(chart_, ser_p_last, power.get_avg());
        lv_chart_set_next_value(chart_, ser_p_avg, power.get_avg_w());

        if (chart->ymin[1] != powerm.get_min() - offset || chart->ymax[1] != powerm.get_max() + offset)
        {
            lv_chart_set_range(chart_, LV_CHART_AXIS_SECONDARY_Y, powerm.get_min() - offset, powerm.get_max() + offset);
        }
    }
}

static void clear_default_style(lv_obj_t *chart)
{
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_obj_set_style_radius(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(chart, 0, 0);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    lv_obj_refresh_ext_draw_size(chart);
    // lv_chart_set_div_line_count(chart, 8, 10);
    lv_chart_set_div_line_count(chart, 0, 0);
}

void vh_create_chart(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, uint16_t points_pr_series)
{
    chart_ = lv_chart_create(parent);
    lv_obj_set_size(chart_, width, height);
    lv_obj_align(chart_, LV_ALIGN_CENTER, 0, -50);

    clear_default_style(chart_);

    lv_chart_set_range(chart_, LV_CHART_AXIS_PRIMARY_X, 0, 100);
    lv_chart_set_range(chart_, LV_CHART_AXIS_SECONDARY_Y, 100, 800);
    lv_chart_set_point_count(chart_, points_pr_series);

    ser_hr_last = lv_chart_add_series(chart_, lv_color_hex(0x990000), LV_CHART_AXIS_PRIMARY_Y);
    ser_p_last = lv_chart_add_series(chart_, lv_color_hex(0x4C3D3D), LV_CHART_AXIS_SECONDARY_Y);

    ser_hr_avg = lv_chart_add_series(chart_, lv_color_hex(0xff0000), LV_CHART_AXIS_PRIMARY_Y);
    ser_p_avg = lv_chart_add_series(chart_, lv_color_hex(0xFFD95A), LV_CHART_AXIS_SECONDARY_Y);

    lv_obj_add_event_cb(chart_, update_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(MSG_NEW_HR, chart_, 0);

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_recolor(label, true);
    lv_label_set_text(label, "#ff0000 HR avg3s# #990000 HR last# & #FFD95A P avgw3s # #4a3939 P avg3s #");
    lv_obj_set_style_text_font(label, font_small, 0);
    lv_obj_align_to(label, chart_, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}
