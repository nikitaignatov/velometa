#include "position_slider.hpp"
#include "Arduino.h"

void PositionSlider::event_cb(lv_event_t *e)
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
            if (measurement_t::position_mm == v->measurement)
            {
                int r = (int32_t)v->value;
                r = r < 350 ? 350 : r;
                r = r > 600 ? 600 : r;
                auto opa_high = r <= 350 || r >= 600;
                r = (uint16_t)map(r, 600, 350, 0, 128);
                auto is_low = lv_obj_get_style_border_opa(obj, 0) == LV_OPA_60;
                if (opa_high && is_low)
                {
                    lv_obj_set_style_border_opa(obj, LV_OPA_100, 0);
                }
                else if (!opa_high && !is_low)
                {
                    lv_obj_set_style_border_opa(obj, LV_OPA_60, 0);
                }
                lv_obj_set_style_border_color(obj, lv_color_hsv_to_rgb(r, 100, opa_high ? 100 : 50), 0);
                lv_obj_set_style_bg_color(obj, lv_color_hsv_to_rgb(r, 100, 100), LV_PART_INDICATOR);
                lv_bar_set_value(obj, (int32_t)v->value, LV_ANIM_OFF);
            }
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

void PositionSlider::subscribe(measurement_t msg_id)
{
    if (obj != nullptr)
    {
        lv_msg_subsribe_obj(msg_id + 100, obj, 0);
    }
}

PositionSlider::PositionSlider(lv_obj_t *parent, measurement_t event_id, const char *label_text, const char *value_text)
    : UI_Base()
{
    if (parent == nullptr)
    {
        return;
    }

    static lv_style_t style_indic;
    static lv_style_t style_bg;

    lv_style_init(&style_bg);
    lv_style_set_bg_opa(&style_bg, LV_OPA_30);
    lv_style_set_bg_color(&style_bg, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_grad_color(&style_bg, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_dir(&style_bg, LV_GRAD_DIR_HOR);
    lv_style_set_border_width(&style_bg, 2);
    lv_style_set_border_opa(&style_bg, LV_OPA_60);
    lv_style_set_border_color(&style_bg, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_pad_all(&style_bg, 6); /*To make the indicator smaller*/
    lv_style_set_radius(&style_bg, 15);

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_HOR);
    lv_style_set_radius(&style_indic, 3);

    obj = lv_bar_create(parent);
    set_obj(obj);

    lv_obj_set_size(obj, 280, 30);
    lv_obj_add_style(obj, &style_bg, 0);
    lv_obj_add_style(obj, &style_indic, LV_PART_INDICATOR);
    lv_bar_set_range(obj, 350, 600);
    lv_bar_set_value(obj, 300, LV_ANIM_OFF);
    set_callback([&](auto *e)
                 { event_cb(e); }, LV_EVENT_MSG_RECEIVED);
    subscribe(event_id);
}