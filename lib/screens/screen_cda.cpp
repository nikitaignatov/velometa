#include "screen_cda.hpp"

lv_obj_t *create_metric(lv_obj_t *parent, const char *label_text, const char *value_text)
{
  auto obj = lv_obj_create(parent);
  lv_obj_set_width(obj, lv_pct(100));
  lv_obj_set_height(obj, 20);

  auto label = lv_label_create(obj);
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_width(label, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(label, LV_SIZE_CONTENT); /// 1
  lv_label_set_text(label, label_text);

  auto value = lv_label_create(obj);
  lv_obj_align(value, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_width(value, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(value, LV_SIZE_CONTENT); /// 1
  lv_label_set_text(value, value_text);
  return obj;
}

void vm_show_cda_panel(lv_obj_t *parent)
{
  auto ui_wind_direction = lv_obj_create(parent);
  lv_obj_set_width(ui_wind_direction, lv_pct(100));
  lv_obj_set_height(ui_wind_direction, lv_pct(100));
  lv_obj_set_align(ui_wind_direction, LV_ALIGN_TOP_MID);
  lv_obj_clear_flag(ui_wind_direction, LV_OBJ_FLAG_SCROLLABLE); /// Flags
  lv_obj_set_style_radius(ui_wind_direction, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_wind_direction, lv_color_hex(0x0E0E0E), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(ui_wind_direction, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_wind_direction, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  auto gs = create_metric(parent, "Ground Speed: ", "0 km/h");
  auto as = create_metric(parent, "Air Speed: ", "0 km/h");
  auto ws = create_metric(parent, "Wind Speed: ", "0 km/h");
  auto ad = create_metric(parent, "Air Density: ", "0 kg/m3");
  auto cda = create_metric(parent, "CdA: ", "0");
  auto a = create_metric(parent, "Altitude: ", "0 m");
  auto p = create_metric(parent, "Air Pressure: ", "0 hPa");
  auto t = create_metric(parent, "Air Temperature: ", "0 C");
  auto rh = create_metric(parent, "Humidity: ", "0 %");
  auto lp = create_metric(parent, "Left Pressure: ", "0 Pa");
  auto rp = create_metric(parent, "Right Pressure: ", "0 Pa");
  auto wy = create_metric(parent, "Wind Yaw: ", "0 deg");
  // auto g = create_metric(parent, "Ax: ", "0 g");
  // auto g = create_metric(parent, "Ay: ", "0 g");
  // auto g = create_metric(parent, "Az: ", "0 g");
  // auto g = create_metric(parent, "Gx: ", "0 ");
  // auto g = create_metric(parent, "Gy: ", "0 ");
  // auto g = create_metric(parent, "Gz: ", "0 ");
}