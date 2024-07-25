#include "measurement.hpp"

void Measurement::event_cb(lv_event_t *e)
{
    auto m = lv_event_get_msg(e);
    auto value_text = (const char *)lv_event_get_user_data(e);
    if (!m)
    {
        ESP_LOGE("TAG", "label_event_cb::msg::empty");
        return;
    }

    if (true || m->id == measurement_t::air_pressure_abs)
    {
        auto p = lv_msg_get_payload(m);
        if (!p)
        {
            ESP_LOGE("TAG", "msg payload is missing");
            return;
        }
        auto v = (raw_measurement_msg_t *)p;
        if (v)
        {
            lv_label_set_text(e->current_target, fmt::format("{}", v->value).c_str());
        }
        else
        {
            ESP_LOGE("TAG", "gps data is invalid or outdated.");
        }
    }
    else
    {
        ESP_LOGE("TAG", "unexpected event cda %i", m->id);
    }
}

void Measurement::subscribe(measurement_t msg_id)
{
    lv_msg_subsribe_obj(msg_id + 100, value, 0);
}

Measurement::Measurement(lv_obj_t *parent, measurement_t event_id, const char *label_text, const char *value_text)
    : UI_Base()
{
    obj = lv_obj_create(parent);
    lv_obj_set_width(obj, lv_pct(45));
    lv_obj_set_height(obj, 50);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(obj, 0, 0);
    lv_obj_set_style_pad_right(obj, 0, 0);
    lv_obj_set_style_pad_top(obj, 0, 0);
    lv_obj_set_style_pad_bottom(obj, 0, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);

    label = lv_label_create(obj);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_width(label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_style_text_font(label, font_xsmall, 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_0, 0);
    lv_label_set_text(label, label_text);

    value = lv_label_create(obj);
    lv_obj_align(value, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_width(value, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(value, LV_SIZE_CONTENT); /// 1
    lv_obj_set_style_text_font(value, font_large, 0);
    lv_obj_set_style_bg_opa(value, LV_OPA_0, 0);
    lv_label_set_text(value, value_text);
    value_fmt = value_text;
    set_obj(value);
    set_callback([&](auto *e)
                 { event_cb(e); }, LV_EVENT_MSG_RECEIVED);
    subscribe(event_id);
}