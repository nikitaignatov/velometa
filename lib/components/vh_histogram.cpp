#include "vh_chart.hpp"

static lv_obj_t *histogram;

static lv_chart_series_t *ser_hr_hist;
static lv_chart_series_t *ser_hr_zone_hist;
static lv_chart_cursor_t *cursor;

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

static void histogram_update_event_cb(lv_event_t *e)
{
    lv_chart_t *chart = (lv_chart_t *)histogram;

    auto activity = current_activity();
    auto total = activity->get_tick();
    uint16_t max_value;
    for (auto i : activity->get_hr_zone_hist())
    {
        auto value = i.second;
        max_value = std::max(max_value, value);
        lv_chart_set_next_value(histogram, ser_hr_hist, value);
        printf("e:%u, v:%u",i.first,i.second);
    }

    lv_chart_set_range(histogram, LV_CHART_AXIS_PRIMARY_Y, 0, max_value);
}

void vh_create_histogram(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, uint16_t points_pr_series)
{
    histogram = lv_chart_create(parent);
    lv_chart_set_type(histogram, LV_CHART_TYPE_BAR);
    lv_obj_set_size(histogram, width, height);
    lv_obj_center(histogram);
    // lv_chart_set_axis_tick(histogram, LV_CHART_AXIS_PRIMARY_X, 10, 0, 5, 0, true, 30);

    clear_default_style(histogram);
    lv_chart_set_point_count(histogram, points_pr_series);

    ser_hr_hist = lv_chart_add_series(histogram, lv_color_hex(0xff0000), LV_CHART_AXIS_PRIMARY_Y);

    lv_obj_add_event_cb(histogram, histogram_update_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(MSG_NEW_HR, histogram, 0);
}
