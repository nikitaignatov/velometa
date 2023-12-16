#include "vh_metric.hpp"

static lv_style_t style_metric_value;
static lv_style_t style_metric_label;

int zone_from_hr(float value)
{
    return calculate_hr_zone(value);
}

int zone_from_power(float value)
{
    return calculate_power_zone(value, FTP);
}

void MetricW::init(lv_obj_t *parent)
{
    lv_style_init(&style_metric_label);
    lv_style_init(&style_metric_value);
    lv_style_set_text_font(&style_metric_label, font_small);
    lv_style_set_text_font(&style_metric_value, font_xlarge);

    container = vh_create_container(parent, width, height);
    lv_obj_set_size(container, width, height);

    value = lv_label_create(container);
    lv_obj_add_style(value, &style_metric_value, 0);
    lv_label_set_recolor(value, true);
    lv_label_set_text(value, "#999999 ---#");

    min = lv_label_create(container);
    lv_label_set_recolor(min, true);
    lv_label_set_text(min, "#999999 ---#");
    lv_obj_set_size(min, 0, 0);

    max = lv_label_create(container);
    lv_label_set_recolor(max, true);
    lv_label_set_text(max, "#999999 ---#");
    lv_obj_align(max, LV_ALIGN_TOP_MID, 0, 0);

    avg = lv_label_create(container);
    lv_label_set_recolor(avg, true);
    lv_label_set_text(avg, "#999999 ---#");

    zone = lv_label_create(container);
    lv_label_set_recolor(zone, true);
    lv_label_set_text(zone, "#999999 -#");
    lv_obj_add_style(zone, &style_metric_value, 0);

    label = lv_label_create(container);
    lv_obj_add_style(label, &style_metric_label, 0);
    lv_label_set_recolor(label, true);
    set_opposite(opposite);
}

static int zone_color(int zone, std::string f)
{
    switch (zone)
    {
    case 1:
        return 0x999999;
    case 2:
        return 0x058ed9;
    case 3:
        return 0x679436;
    case 4:
        return 0xf1c40f;
    case 5:
        return 0xdd2d4a;
    case 6:
        return 0x880d1e;
    case 7:
        return 0x57167a;

    default:
        return 0x999999;
    }
}

void MetricW::update(window_counter_t *metric)
{
    Serial.println("update start");
    auto zone_value = zone_converter(metric->last);
    lv_label_set_text(value, fmt::format("{:.0f}", metric->last).c_str());

    lv_label_set_text(min, fmt::format("{:.0f}", metric->max).c_str());
    lv_label_set_text(max, fmt::format("{:.0f}", metric->max).c_str());
    zone_value = zone_converter(metric->avg);
    auto color = zone_color(zone_value, "{:.0f}");
    lv_label_set_text(avg, fmt::format("{:.0f}", metric->avg).c_str());
    lv_obj_set_style_text_color(avg, lv_color_hex(color), 0);

    zone_value = zone_converter(metric->last);
    color = zone_color(zone_value, "{}");
    lv_label_set_text(zone, fmt::format("{}", zone_value).c_str());
    lv_obj_set_style_bg_color(zone, lv_color_hex(color), 0);
    Serial.println("update2");
}

/**
 * @brief static callback wrapper that will forward the event to the update function.
 * 
 * @param e 
 */
static void metric_event_cb(lv_event_t *e)
{
    auto msg = lv_event_get_msg(e);
    auto metric = (MetricW *)lv_msg_get_user_data(msg);
    auto value = (window_counter_t *)lv_msg_get_payload(msg);
    metric->update(value);
}

void MetricW::subscribe(uint32_t msg_id)
{
    auto that = this;
    lv_obj_add_event_cb(container, metric_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(msg_id, container, that);
}

void MetricW::set_opposite(bool input)
{
    this->opposite = input;
    lv_obj_align(value, opposite ? LV_ALIGN_BOTTOM_LEFT : LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_align(min, opposite ? LV_ALIGN_TOP_LEFT : LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_align(avg, opposite ? LV_ALIGN_TOP_RIGHT : LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_align(zone, opposite ? LV_ALIGN_BOTTOM_RIGHT : LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_align(label, opposite ? LV_ALIGN_TOP_LEFT : LV_ALIGN_TOP_RIGHT, 0, 0);
}

void MetricW::set_label(std::string value)
{
    lv_label_set_text(label, value.c_str());
}

void MetricW::set_zone_converter(std::function<int(float)> converter)
{
    this->zone_converter = converter;
}

lv_obj_t *MetricW::get_object()
{
    return this->container;
}
