#include "vh_screen_brightness.hpp"

static const uint8_t min_brightness = 1;
static const uint8_t max_brightness = 255;

static void slider_event_cb(lv_event_t *e)
{
  lv_obj_t *slider = lv_event_get_target(e);
  uint8_t value = (uint8_t)lv_slider_get_value(slider);
  if (value < min_brightness)
  {
    value = min_brightness;
  }
  else if (value > max_brightness)
  {
    value = max_brightness;
  }
  lcd.setBrightness(value);
}

void vh_create_brightness_slider(lv_obj_t *parent)
{
  lv_obj_t *slider = lv_slider_create(parent);
  lv_obj_center(slider);
  lv_slider_set_range(slider, min_brightness, max_brightness);
  lv_obj_set_size(slider, 20, 300);
  lv_slider_set_value(slider, max_brightness / 2, lv_anim_enable_t::LV_ANIM_OFF);
  lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, "Set Brightness");
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
}