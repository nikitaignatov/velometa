#include "screen_cda.hpp"
#include "Arduino.h"
static lv_obj_t *obj;
static void label_event_cb(lv_event_t *e)
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
      lv_label_set_text(e->current_target, fmt::format(value_text == nullptr ? "{}" : value_text, v->value).c_str());
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

lv_obj_t *create_metric(lv_obj_t *parent, const char *label_text, const char *value_text, measurement_t event_id)
{
  auto obj = lv_obj_create(parent);
  lv_obj_set_width(obj, lv_pct(45));
  lv_obj_set_height(obj, 50);
  lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_left(obj, 0, 0);
  lv_obj_set_style_pad_right(obj, 0, 0);
  lv_obj_set_style_pad_top(obj, 0, 0);
  lv_obj_set_style_pad_bottom(obj, 0, 0);

  auto label = lv_label_create(obj);
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_width(label, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(label, LV_SIZE_CONTENT); /// 1
  lv_obj_set_style_text_font(label, font_small, 0);
  lv_label_set_text(label, label_text);

  auto value = lv_label_create(obj);
  lv_obj_align(value, LV_ALIGN_TOP_RIGHT, 0, 0);
  lv_obj_set_width(value, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(value, LV_SIZE_CONTENT); /// 1
  lv_obj_set_style_text_font(value, font_large, 0);
  lv_label_set_text(value, value_text);
  lv_obj_add_event_cb(value, label_event_cb, LV_EVENT_MSG_RECEIVED, (void *)value_text);
  lv_msg_subsribe_obj(event_id + 100, value, 0);
  // lv_label_set_recolor(value, true);

  return obj;
}

void vm_show_cda_panel(lv_obj_t *parent)
{
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_flex_flow(&style, LV_FLEX_FLOW_ROW_WRAP);
  lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_layout(&style, LV_LAYOUT_FLEX);

  lv_obj_t *cont = lv_obj_create(parent);
  lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
  lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(cont);
  lv_obj_add_style(cont, &style, 0);

  auto pw = create_metric(cont, "Power\n[ watt ]", "{:.1f}", measurement_t::power);
  auto ws = create_metric(cont, "W Speed\n[ km/h ]", "{:.1f}", measurement_t::wind_speed);
  auto gs = create_metric(cont, "G Speed\n[ km/h ]", "{:.1f}", measurement_t::speed);
  auto as = create_metric(cont, "A Speed\n[ km/h ]", "{:.1f}", measurement_t::air_speed);
  // auto ws = create_metric(cont, "Wind Speed: ", "0 km/h", measurement_t::air_pressure_abs);
  // auto cda = create_metric(cont, "CdA: ", "0", measurement_t::air_pressure_abs);
  auto lp = create_metric(cont, "LP\n[ Pa ]", "{:.1f}", measurement_t::diff_pressure_l_pa);
  auto rp = create_metric(cont, "RP\n[ Pa ]", "{:.1f}", measurement_t::diff_pressure_r_pa);
  // auto wy = create_metric(cont, "Wind Yaw: ", "0 deg", measurement_t::air_pressure_abs);

  auto ad = create_metric(cont, "Air D\n[ kg/m3 ]", "{:.5f}", measurement_t::air_density);
  auto p = create_metric(cont, "Air P\n[ hPa ]", "{:.0f}", measurement_t::air_pressure_abs);
  auto t = create_metric(cont, "Air T\n[ C ]", "{:.2f}", measurement_t::air_temperature);
  auto rh = create_metric(cont, "RH\n[ % ]", "{:.2f}", measurement_t::air_humidity);

  // auto x = create_metric(cont, "Roll: ", "{:.1f}", measurement_t::roll);
  // auto y = create_metric(cont, "Pitch: ", "{:.1f}", measurement_t::pitch);
  auto z = create_metric(cont, "Dist\n[ mm ]: ", "{:.1f}", measurement_t::position_mm);

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

  /*Create an object with the new style_pr*/
  obj = lv_bar_create(cont);
  lv_obj_set_size(obj, 280, 30);
  lv_obj_add_style(obj, &style_bg, 0);
  lv_obj_add_style(obj, &style_indic, LV_PART_INDICATOR);
  lv_bar_set_range(obj, 350, 600);
  lv_bar_set_value(obj, 300, LV_ANIM_OFF);
  // auto wy = create_metric(cont, "Wind Yaw: ", "Wind Yaw: {} %", measurement_t::wind_yaw);
  // auto a = create_metric(cont, "Altitude: ", "Altitude: {} m", measurement_t::elevation);

  // auto g = create_metric(cont, "Gx: ", "0 ");
  // auto g = create_metric(cont, "Gy: ", "0 ");
  // auto g = create_metric(cont, "Gz: ", "0 ");
}