#include "chart.hpp"

void Chart::event_cb(lv_event_t *e)
{
    ESP_LOGW("Chart", "update_event_cb.start");
    if (obj == nullptr)
    {
        ESP_LOGE("Chart", "update_event_cb.obj.null");
        return;
    }
    auto chart = (lv_chart_t *)obj;

    uint32_t offset = 2;

    auto activity = current_activity();
    if (current_activity == nullptr)
    {
        ESP_LOGW("Chart", "update_event_cb.activity.null");
        return;
    }
    ESP_LOGW("Chart", "update_event_cb.activity.get.values");
    auto hr = activity->get_hr(3);
    auto hrm = activity->get_hr(8);
    auto power = activity->get_power(2);
    auto powerm = activity->get_power(9);
    if (hr.count > 0)
    {
        if (series_primary == nullptr)
        {
            ESP_LOGW("Chart", "update_event_cb.hr.ser_hr_last.null");
            return;
        }

        lv_chart_set_next_value(obj, series_primary, hr.get_last());
        lv_chart_set_next_value(obj, series_secondary, hr.get_avg());

        if (chart->ymin[0] > hrm.get_min() - offset || chart->ymax[0] < hrm.get_max() + offset)
        {
            lv_chart_set_range(obj, LV_CHART_AXIS_PRIMARY_Y, hrm.get_min() - offset, hrm.get_max() + offset);
        }
    }
    // if (power.count > 0)
    // {
    //     lv_chart_set_next_value(obj, ser_p_last, power.get_avg());
    //     lv_chart_set_next_value(obj, ser_p_avg, power.get_avg_w());

    //     if (chart->ymin[1] != powerm.get_min() - offset || chart->ymax[1] != powerm.get_max() + offset)
    //     {
    //         lv_chart_set_range(obj, LV_CHART_AXIS_SECONDARY_Y, powerm.get_min() - offset, powerm.get_max() + offset);
    //     }
    // }
    ESP_LOGW("Chart", "update_event_cb.done");
}

static void clear_default_style(lv_obj_t *chart)
{
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_obj_set_style_radius(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(chart, 0, 0);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    lv_obj_refresh_ext_draw_size(chart);
    lv_chart_set_div_line_count(chart, 0, 0);
}

static void set_chart_line_width(lv_obj_t *chart, uint16_t width)
{
    lv_obj_set_style_line_width(chart, 4, LV_PART_ITEMS);
    lv_obj_set_style_size(chart, 3, LV_PART_INDICATOR);
}

void Chart::subscribe(measurement_t msg_id)
{
    lv_msg_subsribe_obj(msg_id, obj, 0);
}

void Chart::add_primary_series(measurement_t msg_id, uint32_t color, uint16_t min, uint16_t max)
{
    series_primary = lv_chart_add_series(obj, lv_color_hex(color), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_range(obj, LV_CHART_AXIS_PRIMARY_X, min, max);
}

void Chart::add_secondary_series(measurement_t msg_id, uint32_t color, uint16_t min, uint16_t max)
{
    series_secondary = lv_chart_add_series(obj, lv_color_hex(color), LV_CHART_AXIS_SECONDARY_Y);
    lv_chart_set_range(obj, LV_CHART_AXIS_SECONDARY_Y, min, max);
}

Chart::Chart(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, uint16_t points_pr_series) : UI_Base()
{
    obj = lv_chart_create(parent);
    set_obj(obj);
    set_callback([&](auto *e)
                 { event_cb(e); }, LV_EVENT_MSG_RECEIVED);
    subscribe(measurement_t::heartrate);

    lv_obj_set_size(obj, width, height);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, -50);

    clear_default_style(obj);
    set_chart_line_width(obj, 4);

    lv_chart_set_point_count(obj, points_pr_series);

    add_secondary_series(measurement_t::heartrate, 0x990000, 0, 100);
    add_primary_series(measurement_t::heartrate, 0xff0000, 0, 100);

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_recolor(label, true);
    lv_label_set_text(label, "#ff0000 HR avg3s# #990000 HR last# & #FFD95A P avgw3s # #4a3939 P avg3s #");
    lv_obj_set_style_text_font(label, font_small, 0);
    lv_obj_align_to(label, obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}
