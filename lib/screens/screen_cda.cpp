#include "screen_cda.hpp"


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

  auto x = create_metric(cont, "Ax: ", "{:.3f}", measurement_t::ax_ms2);
  auto y = create_metric(cont, "Ay: ", "{:.3f}", measurement_t::ay_ms2);
  auto z = create_metric(cont, "Az: ", "{:.3f}", measurement_t::az_ms2);
  // auto wy = create_metric(cont, "Wind Yaw: ", "Wind Yaw: {} %", measurement_t::wind_yaw);
  // auto a = create_metric(cont, "Altitude: ", "Altitude: {} m", measurement_t::elevation);

  // auto g = create_metric(cont, "Gx: ", "0 ");
  // auto g = create_metric(cont, "Gy: ", "0 ");
  // auto g = create_metric(cont, "Gz: ", "0 ");
}