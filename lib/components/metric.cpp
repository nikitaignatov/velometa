#include "metric.hpp"

static lv_style_t style_metric_value;
static lv_style_t style_metric_label;

static std::map<int, lv_color_t> zone_color_map{
    {0, lv_color_hex(0x999999)},
    {1, lv_color_hex(0x999999)},
    {2, lv_color_hex(0x058ed9)},
    {3, lv_color_hex(0x679436)},
    {4, lv_color_hex(0xf1c40f)},
    {5, lv_color_hex(0xdd2d4a)},
    {6, lv_color_hex(0x880d1e)},
    {7, lv_color_hex(0x57167a)},
    {8, lv_color_hex(0xff0000)},
    {9, lv_color_hex(0xff0000)},
};

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
    lv_style_set_text_font(&style_metric_value, font_xxlarge_bold);

    _container = new Container(parent, width, height);
    container = _container->get_obj();

    lv_obj_set_size(container, width, height);

    value = lv_label_create(container);
    lv_obj_add_style(value, &style_metric_value, 0);
    // lv_label_set_recolor(value, true);
    lv_label_set_text(value, "---");

    min = lv_label_create(container);
    // lv_label_set_recolor(min, true);
    lv_label_set_text(min, "---");
    lv_obj_set_size(min, 0, 0);

    max = lv_label_create(container);
    // lv_label_set_recolor(max, true);
    lv_label_set_text(max, "---");
    lv_obj_align(max, LV_ALIGN_TOP_MID, 0, 0);

    avg = lv_label_create(container);
    // lv_label_set_recolor(avg, true);
    lv_label_set_text(avg, "---");

    zone = lv_label_create(container);
    // lv_label_set_recolor(zone, true);
    lv_label_set_text(zone, "-");
    lv_obj_add_style(zone, &style_metric_value, 0);

    label = lv_label_create(container);
    lv_obj_add_style(label, &style_metric_label, 0);
    // lv_label_set_recolor(label, true);
    set_opposite(opposite);
}

void MetricW::update(window_counter_t *metric)
{
    ESP_LOGD("metric_update", "begin");
    lv_label_set_text(value, fmt::format("{}", metric->get_last()).c_str());

    lv_label_set_text(min, fmt::format("{}", metric->get_min()).c_str());
    lv_label_set_text(max, fmt::format("{}", metric->get_max()).c_str());
    auto zone_value = zone_converter(metric->get_avg());
    auto color = zone_color_map[zone_value];
    lv_label_set_text(avg, fmt::format("{}", metric->get_avg()).c_str());
    lv_obj_set_style_text_color(avg, color, 0);

    zone_value = zone_converter(metric->get_last());
    color = zone_color_map[zone_value];
    lv_label_set_text(zone, fmt::format("{}", zone_value).c_str());
    lv_obj_set_style_bg_color(zone, color, 0);
    ESP_LOGD("metric_update", "end");
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
    lv_obj_add_event_cb(container, metric_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(msg_id, container, this);
}

void MetricW::set_opposite(bool input)
{
    this->opposite = input;
    lv_obj_align(value, opposite ? LV_ALIGN_BOTTOM_LEFT : LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    // lv_obj_align(min, opposite ? LV_ALIGN_TOP_LEFT : LV_ALIGN_TOP_RIGHT, 0, 0);
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
