#include <stdint.h>
#include <vector>
#include <string>
#include <float.h>
// #define LGFX_AUTODETECT // Autodetect board
#define LV_CONF_INCLUDE_SIMPLE
#define LV_USE_THEME_NIGHT 1
/* Uncomment below line to draw on screen with touch */
#define DRAW_ON_SCREEN

#include "lgfx.hpp"

static LGFX lcd; // declare display variable

#include <lvgl.h>
#include "lv_conf.h"

/*** Setup screen resolution for LVGL ***/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 480;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

// Variables for touch x,y
#ifdef DRAW_ON_SCREEN
static int32_t x, y;
#endif

/*** Function declaration ***/
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void lv_button_demo(void);
void lv_example_get_started_1(void);
void lv_example_tileview_1(void);

lv_obj_t *hrv;
lv_obj_t *power;
lv_obj_t *speed;

void update_hr(std::string);
void update_power(std::string);
void update_speed(std::string);

static lv_style_t style_metric_label;
static lv_style_t style_metric_value;

void vh_setup(void)
{
  Serial.begin(115200); /* prepare for possible serial debug */
  lcd.init();           // Initialize LovyanGFX
  lv_init();            // Initialize lvgl

  // Setting display to landscape
  if (lcd.width() < lcd.height())
    lcd.setRotation(2);
  lcd.setBrightness(128);

  /* LVGL : Setting up buffer to use for display */
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*** LVGL : Setup & Initialize the display device driver ***/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = display_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*** LVGL : Setup & Initialize the input device driver ***/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read;
  lv_indev_drv_register(&indev_drv);

  lv_example_tileview_1();
}

lv_obj_t *vh_show_value(lv_obj_t *tile, std::string label, std::string value, lv_align_t align)
{
  auto font_small = &lv_font_montserrat_10;
  auto font_med = &lv_font_montserrat_14;
  auto font_large = &lv_font_montserrat_32;
  auto font_xlarge = &lv_font_montserrat_48;
  lv_style_init(&style_metric_label);
  lv_style_init(&style_metric_value);

  lv_style_set_text_font(&style_metric_label, font_small);
  lv_style_set_text_font(&style_metric_value, font_xlarge);

  lv_obj_t *lbl = lv_label_create(tile);
  lv_obj_add_style(lbl, &style_metric_label, 0);
  lv_label_set_recolor(lbl, true);
  lv_label_set_text(lbl, label.c_str());
  lv_obj_align(lbl, align, 0, 2);

  lv_obj_t *metric = lv_label_create(tile);
  lv_obj_add_style(metric, &style_metric_value, 0);
  lv_label_set_recolor(metric, true);
  lv_label_set_text(metric, value.c_str());
  lv_obj_align(metric, align, 0, 16);

  return metric;
}

void update_value(lv_obj_t *target, std::string text)
{
  lv_label_set_text(target, text.c_str());
}

void update_hr(std::string v)
{
  update_value(hrv, v);
}
void update_power(std::string v)
{
  update_value(power, v);
}
void update_speed(std::string v)
{
  update_value(speed, v);
}

lv_obj_t *vh_create_tile(lv_obj_t *tv)
{
  lv_obj_t *tile = lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM);
  lv_obj_set_style_bg_color(tile, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(tile, LV_OPA_100, 0);
  return tile;
}

void lv_example_tileview_1(void)
{
  lv_obj_t *tv = lv_tileview_create(lv_scr_act());

  lv_obj_t *tile1 = vh_create_tile(tv);
    
  power = vh_show_value(tile1, "#666666 Power [Watt]#", "#ffffff ----#", LV_ALIGN_TOP_LEFT);
  hrv = vh_show_value(tile1, "#666666 HR [bpm]#", "#999999 ---#", LV_ALIGN_TOP_MID);
  speed = vh_show_value(tile1, "#666666 Speed [km/h]#", "#999999 --.-#", LV_ALIGN_TOP_RIGHT);
}

void vh_loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);

  if (lcd.getTouch(&x, &y))
  {
    lcd.fillRect(x - 2, y - 2, 2, 2, TFT_RED);
    lcd.setCursor(0, 0);
    lcd.printf("Touch w:(%03d,%03d)", x, y);
  }
}

/*** Display callback to flush the buffer to screen ***/
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lcd.startWrite();
  lcd.setAddrWindow(area->x1, area->y1, w, h);
  lcd.pushColors((uint16_t *)&color_p->full, w * h, true);
  lcd.endWrite();

  lv_disp_flush_ready(disp);
}

/*** Touchpad callback to read the touchpad ***/
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;
  bool touched = lcd.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    // Serial.printf("Touch (x,y): (%03d,%03d)\n",touchX,touchY );
  }
}
