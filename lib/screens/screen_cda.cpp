#include "screen_cda.hpp"

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

  auto pw = new Measurement(cont, measurement_t::power, "Power\nwatt", "{:.1f}");
  auto ws = new Measurement(cont, measurement_t::wind_speed, "W Speed\nkm/h", "{:.1f}");
  auto gs = new Measurement(cont, measurement_t::speed, "G Speed\nkm/h", "{:.1f}");
  auto as = new Measurement(cont, measurement_t::air_speed, "A Speed\nkm/h", "{:.1f}");
  auto lp = new Measurement(cont, measurement_t::diff_pressure_l_pa, "LP\nPa", "{:.1f}");
  auto rp = new Measurement(cont, measurement_t::diff_pressure_r_pa, "RP\nPa", "{:.1f}");
  auto ad = new Measurement(cont, measurement_t::air_density, "Air D\nkg/m3", "{:.5f}");
  auto ap = new Measurement(cont, measurement_t::air_pressure_abs, "Air P\nhPa", "{:.0f}");
  auto at = new Measurement(cont, measurement_t::air_temperature, "Air T\nC", "{:.2f}");
  auto rh = new Measurement(cont, measurement_t::air_humidity, "RH\n%", "{:.2f}");
  auto r = new Measurement(cont, measurement_t::roll, "Roll", "{:.1f}");
  auto p = new Measurement(cont, measurement_t::pitch, "Pitch", "{:.1f}");
  auto pos = new PositionSlider(cont, measurement_t::position_mm, "Dist\nmm", "{:.1f}");
  // auto pos = new DistanceMap(cont, 1337, "Dist\nmm", "{:.1f}");

  // auto wy = create_metric(cont, "Wind Yaw: ", "Wind Yaw: {} %", measurement_t::wind_yaw);
  // auto a = create_metric(cont, "Altitude: ", "Altitude: {} m", measurement_t::elevation);
}