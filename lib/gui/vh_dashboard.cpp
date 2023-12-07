#include "vh_dashboard.hpp"

static void label_event_cb(lv_event_t *e);

static lv_style_t style_metric_value;
static lv_style_t style_metric_label;
lv_obj_t *hrv;
lv_obj_t *hrv_l;
lv_obj_t *_power;
lv_obj_t *_speed;
lv_obj_t *list;
lv_obj_t *label_m;

void update_value(lv_obj_t *target, std::string text) { lv_label_set_text(target, text.c_str()); }
void update_hr(int min, int max, int avg, int value, int zone, bool connected) {}
void update_power(std::string v) { update_value(_power, v); }
void update_speed(std::string v) { update_value(_speed, v); }

lv_obj_t *vh_show_value(lv_obj_t *tile, std::string label, std::string value, lv_align_t align)
{
    lv_obj_t *lbl = lv_label_create(tile);
    lv_obj_add_style(lbl, &style_metric_label, 0);
    lv_label_set_recolor(lbl, true);
    lv_label_set_text(lbl, label.c_str());
    if (LV_ALIGN_BOTTOM_LEFT == align)
        lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 0, 0);
    if (LV_ALIGN_BOTTOM_RIGHT == align)
        lv_obj_align(lbl, LV_ALIGN_TOP_RIGHT, 0, 0);
    if (LV_ALIGN_BOTTOM_MID == align)
        lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *metric = lv_label_create(tile);
    lv_obj_add_style(metric, &style_metric_value, 0);
    lv_label_set_recolor(metric, true);
    lv_label_set_text(metric, value.c_str());
    lv_obj_align(metric, align, 0, 0);

    return metric;
}

lv_obj_t *vh_show_hr(lv_obj_t *tile, std::string label, std::string value, lv_align_t align)
{
    lv_obj_t *lbl = lv_label_create(tile);
    lv_obj_add_style(lbl, &style_metric_label, 0);
    lv_label_set_recolor(lbl, true);
    lv_label_set_text(lbl, label.c_str());
    if (LV_ALIGN_BOTTOM_LEFT == align)
        lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 0, 0);
    if (LV_ALIGN_BOTTOM_RIGHT == align)
        lv_obj_align(lbl, LV_ALIGN_TOP_RIGHT, 0, 0);
    if (LV_ALIGN_BOTTOM_MID == align)
        lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *metric = lv_label_create(tile);
    lv_obj_add_style(metric, &style_metric_value, 0);
    lv_label_set_recolor(metric, true);
    lv_label_set_text(metric, value.c_str());
    lv_obj_align(metric, align, 0, 0);

    hrv = metric;
    hrv_l = lbl;

    return metric;
}

void vh_create_dashboard(lv_obj_t *parent)
{
    lv_style_init(&style_metric_label);
    lv_style_init(&style_metric_value);

    lv_obj_t *metrics = vh_create_container(parent, 0, 22, lcd.width(), 100, 0x000000);
    lv_obj_t *metrics_2 = vh_create_container(parent, 0, 200, lcd.width() / 2, 100, 0x000000);
    // lv_obj_set_scrollbar_mode(metrics, LV_SCROLLBAR_MODE_OFF);
    // lv_obj_set_size(metrics, lcd.width(), 70);
    // lv_obj_set_scrollbar_mode(metrics_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(metrics_2, lcd.width() / 2, 70);
    _power = vh_show_value(metrics, "#666666 Power [Watt]#", "#ffffff ----#", LV_ALIGN_BOTTOM_LEFT);
    vh_show_hr(metrics_2, "#aa6666 *HR [bpm]#", "#999999 ---#", LV_ALIGN_BOTTOM_MID);
    // _speed = vh_show_value(metrics, "#666666 Speed [km/h]#", "#999999 --.-#", LV_ALIGN_BOTTOM_RIGHT);

    lv_obj_add_event_cb(_power, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    // lv_obj_add_event_cb(_speed, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_obj_add_event_cb(hrv, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    // lv_msg_subsribe_obj(MSG_NEW_SPEED, _speed, (void *)"{:.1f}");
    lv_msg_subsribe_obj(MSG_NEW_POWER, _power, (void *)"{:.0f}");
    lv_msg_subsribe_obj(MSG_NEW_HR, hrv, (void *)"{:.0f}");
}

static void label_event_cb(lv_event_t *e)
{
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *formatter = (char *)lv_msg_get_user_data(m);
    const metric_info_t *v = (metric_info_t *)lv_msg_get_payload(m);
    lv_label_set_text(label, fmt::format(formatter, v->last).c_str());
}