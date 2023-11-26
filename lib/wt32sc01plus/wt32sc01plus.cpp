#include <fmt/core.h>
#include <fmt/ranges.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <float.h>
#include "fonts/dosis_extra_light10.c"

// #define LGFX_AUTODETECT // Autodetect board
#define LV_CONF_INCLUDE_SIMPLE
#define LV_USE_THEME_NIGHT 1
/* Uncomment below line to draw on screen with touch */
#define DRAW_ON_SCREEN

#include "lgfx.hpp"

static LGFX lcd; // declare display variable

#include <lvgl.h>
#include "lv_conf.h"
#include "vh_container.hpp"
#include "vh_status_bar.hpp"

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
void main_screen(void);

lv_obj_t *vh_bg_black(lv_obj_t *obj)
{
  lv_obj_set_style_bg_color(obj, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(obj, LV_OPA_100, 0);
  return obj;
}
// lv_font_t * my_font;
// my_font = lv_font_load(&Dosis_Light8pt7b);

static auto font_small = &lv_font_montserrat_10;
static auto font_med = &lv_font_montserrat_14;
static auto font_large = &lv_font_montserrat_32;
static auto font_xlarge = &lv_font_montserrat_48;

static lv_style_t style_metric_label;
static lv_style_t style_metric_value;
static lv_style_t style_bg;

lv_obj_t *hrv;
lv_obj_t *hrv_l;
lv_obj_t *power;
lv_obj_t *speed;

void update_value(lv_obj_t *target, std::string text) { lv_label_set_text(target, text.c_str()); }
void update_hr(int min, int max, int avg, int value, int zone, bool connected)
{
  if (connected)
  {
    lv_label_set_text(hrv_l, "#77aa77 HR [bpm]#");
  }
  else
  {
    lv_label_set_text(hrv_l, "#666666 HR [bpm]#");
  }
  if (value > 70)
    update_value(hrv, fmt::format("#ff0000 {}#", value));
  else
    update_value(hrv, fmt::format("#ffffff {}#", value));
}
void update_power(std::string v) { update_value(power, v); }
void update_speed(std::string v) { update_value(speed, v); }

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

  main_screen();
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

static void velohub_theme_callback(lv_theme_t *th, lv_obj_t *obj)
{
  LV_UNUSED(th);
  lv_obj_add_style(obj, &style_bg, 0);
}

static void apply_velohub_theme(void)
{
  lv_style_init(&style_bg);
  lv_style_set_bg_color(&style_bg, lv_color_hex(0x000000));
  lv_style_set_bg_opa(&style_bg, LV_OPA_100);

  /*Initialize the new theme from the current theme*/
  lv_theme_t *th_act = lv_disp_get_theme(NULL);
  static lv_theme_t th_new;
  th_new = *th_act;

  /*Set the parent theme ans the style applay callback for the new theme*/
  lv_theme_set_parent(&th_new, th_act);
  lv_theme_set_apply_cb(&th_new, velohub_theme_callback);

  /*Assign the new theme the the current display*/
  lv_disp_set_theme(NULL, &th_new);
}

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

lv_obj_t *vh_create_tile(lv_obj_t *tv)
{
  lv_obj_t *tile = lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM);
  return tile;
}

void main_screen(void)
{
  apply_velohub_theme();
  LV_FONT_CUSTOM_DECLARE(teko64);

  lv_style_init(&style_metric_label);
  lv_style_init(&style_metric_value);
  lv_style_set_text_font(&style_metric_label, font_small);
  lv_style_set_text_font(&style_metric_value, font_xlarge);

  lv_obj_t *tv = lv_tileview_create(lv_scr_act());
  lv_obj_t *tile1 = vh_create_tile(tv);

  lv_obj_t *status_bar = vh_create_status_bar(tile1, lcd.width());

  /*Tile2: a button*/
  lv_obj_t *tile2 = lv_tileview_add_tile(tv, 0, 1, LV_DIR_TOP | LV_DIR_RIGHT);

  lv_obj_t *btn = lv_btn_create(tile2);

  lv_obj_t *list = lv_list_create(tile2);
  lv_obj_set_size(list, LV_PCT(100), LV_PCT(90));
  lv_obj_set_pos(list, 0, LV_PCT(10));

  lv_list_add_btn(list, NULL, "One");
  lv_list_add_btn(list, NULL, "Two");
  lv_list_add_btn(list, NULL, "Three");
  lv_list_add_btn(list, NULL, "Four");
  lv_list_add_btn(list, NULL, "Five");
  lv_list_add_btn(list, NULL, "Six");
  lv_list_add_btn(list, NULL, "Seven");
  lv_list_add_btn(list, NULL, "Eight");
  lv_list_add_btn(list, NULL, "Nine");
  lv_list_add_btn(list, NULL, "Ten");

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, "start scan");

  lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x999999), 0);
  lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *metrics = vh_create_container(tile1, 0, 22, lcd.width(), 100, 0x000000);
  lv_obj_t *metrics_2 = vh_create_container(tile1, 0, 200, lcd.width() / 2, 100, 0x000000);
  lv_obj_set_scrollbar_mode(metrics, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_size(metrics, lcd.width(), 70);
  lv_obj_set_scrollbar_mode(metrics_2, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_size(metrics_2, lcd.width() / 2, 70);
  power = vh_show_value(metrics, "#666666 Power [Watt]#", "#ffffff ----#", LV_ALIGN_BOTTOM_LEFT);
  vh_show_hr(metrics_2, "#666666 HR [bpm]#", "#999999 ---#", LV_ALIGN_BOTTOM_MID);
  speed = vh_show_value(metrics, "#666666 Speed [km/h]#", "#999999 --.-#", LV_ALIGN_BOTTOM_RIGHT);
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
