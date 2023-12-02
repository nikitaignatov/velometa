#include "wt32sc01plus.hpp"
static LGFX lcd;

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
static lv_style_t style_console;
static lv_style_t style_metric_value;
static lv_style_t style_bg;
lv_obj_t *hrv;
lv_obj_t *hrv_l;
lv_obj_t *_power;
lv_obj_t *_speed;
lv_obj_t *list;
lv_obj_t *label_m;

void update_value(lv_obj_t *target, std::string text) { lv_label_set_text(target, text.c_str()); }
void update_hr(int min, int max, int avg, int value, int zone, bool connected)
{
}
void update_power(std::string v) { update_value(_power, v); }
void update_speed(std::string v) { update_value(_speed, v); }

void vh_setup(void)
{
  // delay(5000);
  Serial.begin(115200); /* prepare for possible serial debug */
  auto r = fs_mount_sd_card();
  Serial.print(r->fsize);
  Serial.println(" init_sdspi");
  lcd.init(); // Initialize LovyanGFX
  lv_init();  // Initialize lvgl
  lv_fs_stdio_init();
  lv_png_init();

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
#ifdef DRAW_ON_SCREEN
  if (lcd.getTouch(&x, &y))
  {
    lcd.fillRect(x - 2, y - 2, 2, 2, TFT_RED);
    lcd.setCursor(0, 0);
    lcd.printf("Touch w:(%03d,%03d)", x, y);
  }
#endif
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

  lv_style_init(&style_console);
  lv_style_set_text_font(&style_console, font_med);

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

void event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
  {
    // sys.mocked = sys.mocked == 1 ? 0 : 1;
    LV_LOG_USER("Clicked");
  }
  else if (code == LV_EVENT_VALUE_CHANGED)
  {
    LV_LOG_USER("Toggled");
  }
}

// static void slider_event_cb(lv_event_t *e)
// {
//   lv_obj_t *slider = lv_event_get_target(e);

//   uint8_t v = (uint8_t)lv_slider_get_value(slider);
//   lcd.setBrightness(v);
// }

// static void map_slider_event_cb(lv_event_t *e)
// {
//   lv_obj_t *slider = lv_event_get_target(e);
//   Serial.print("map_slider_event_cb: ");
//   Serial.print(lv_slider_get_value(slider));
//   Serial.print(" : ");
//   Serial.print((uint16_t)lv_slider_get_value(slider));
//   Serial.print(" = ");
//   Serial.print(map_frequency);

//   map_frequency = (uint16_t)lv_slider_get_value(slider);
//   Serial.print(" => ");
//   Serial.println(map_frequency);
//   lv_label_set_text(label_m, fmt::format("{}", map_frequency).c_str());
// }

// static void slider_event_cb(lv_event_t *e);
// static void label_event_cb(lv_event_t *e);
void main_screen(void)
{
  apply_velohub_theme();

  // lv_style_init(&style_metric_label);
  // lv_style_init(&style_metric_value);

  // lv_style_set_text_font(&style_metric_label, font_small);
  // lv_style_set_text_font(&style_metric_value, font_xlarge);

  lv_obj_t *p = vh_tiles_init(lv_scr_act());

  lv_obj_t *tile1 = vh_get_main_tile();
  lv_obj_t *status_bar = vh_create_status_bar(tile1, lcd.width());

  // lv_obj_t *btn_m = lv_btn_create(tile1);

  // lv_obj_add_event_cb(btn_m, event_handler, LV_EVENT_ALL, NULL);
  // label_m = lv_label_create(btn_m);
  // lv_label_set_text(label_m, "Mock Data");

  // lv_obj_set_size(btn_m, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  // lv_obj_set_style_bg_color(btn_m, lv_color_hex(0x999999), 0);
  // lv_obj_align(btn_m, LV_ALIGN_BOTTOM_MID, 0, 0);

  // /*Tile2: a button*/
  // lv_obj_t *label = lv_label_create(scan_tile);
  // lv_label_set_text(label, "Set Brightness");

  // // lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  // lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

  // list = label;

  // lv_obj_t *metrics = vh_create_container(tile1, 0, 22, lcd.width(), 100, 0x000000);
  // lv_obj_t *metrics_2 = vh_create_container(tile1, 0, 200, lcd.width() / 2, 100, 0x000000);
  // lv_obj_set_scrollbar_mode(metrics, LV_SCROLLBAR_MODE_OFF);
  // lv_obj_set_size(metrics, lcd.width(), 70);
  // lv_obj_set_scrollbar_mode(metrics_2, LV_SCROLLBAR_MODE_OFF);
  // lv_obj_set_size(metrics_2, lcd.width() / 2, 70);
  // _power = vh_show_value(metrics, "#666666 Power [Watt]#", "#ffffff ----#", LV_ALIGN_BOTTOM_LEFT);
  // vh_show_hr(metrics_2, "#aa6666 *HR [bpm]#", "#999999 ---#", LV_ALIGN_BOTTOM_MID);
  // _speed = vh_show_value(metrics, "#666666 Speed [km/h]#", "#999999 --.-#", LV_ALIGN_BOTTOM_RIGHT);

  // lv_obj_add_event_cb(_power, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
  // lv_obj_add_event_cb(_speed, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
  // lv_obj_add_event_cb(hrv, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
  // lv_msg_subsribe_obj(MSG_NEW_SPEED, _speed, (void *)"{:.1f}");
  // lv_msg_subsribe_obj(MSG_NEW_POWER, _power, (void *)"{:.0f}");
  // lv_msg_subsribe_obj(MSG_NEW_HR, hrv, (void *)"{:.0f}");

  // lv_obj_t *slider = lv_slider_create(scan_tile);
  // lv_obj_center(slider);
  // lv_slider_set_range(slider, 1, 255);
  // lv_obj_set_size(slider, 20, 300);
  // lv_slider_set_value(slider, 128, lv_anim_enable_t::LV_ANIM_OFF);
  // lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

  // lv_obj_t *map_update_slider = lv_slider_create(tile1);
  // lv_obj_align(map_update_slider, LV_ALIGN_CENTER, 0, 25);
  // lv_slider_set_range(map_update_slider, 30, 10000);
  // lv_slider_set_value(map_update_slider, map_frequency, lv_anim_enable_t::LV_ANIM_OFF);
  // lv_obj_set_size(map_update_slider, 10, 300);
  // lv_obj_add_event_cb(map_update_slider, map_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

  // vh_map_init(tile1);
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

void update(system_t k)
{
  Serial.println("update sys");
  return;
}
static void label_event_cb(lv_event_t *e)
{
  lv_obj_t *label = lv_event_get_target(e);
  lv_msg_t *m = lv_event_get_msg(e);

  const char *formatter = (char *)lv_msg_get_user_data(m);
  const metric_info_t *v = (metric_info_t *)lv_msg_get_payload(m);
  lv_label_set_text(label, fmt::format(formatter, v->last).c_str());
}

void publish(uint32_t topic, metric_info_t payload)
{
  return;
  if (topic == MSG_NEW_HEADING)
  {
    // auto x = lv_img_get_offset_x(img);
    // auto y = lv_img_get_offset_y(img);
    // if (x > 255)
    // {
    //   x = 0;
    // }
    // else
    // {
    //   x++;
    // }
    // if (y > 254)
    // {
    //   y = 0;
    // }
    // else
    // {
    //   y++;
    //   y++;
    // }
    // lv_img_set_offset_x(img, x);
    // lv_img_set_offset_y(img, y);
  }
  else
  {
    lv_msg_send(topic, &payload);
  }
}