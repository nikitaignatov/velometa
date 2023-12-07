#include "wt32sc01plus.hpp"
LGFX lcd;

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
void main_screen(void);

static auto font_small = &lv_font_montserrat_10;
static auto font_med = &lv_font_montserrat_14;
static auto font_large = &lv_font_montserrat_32;
static auto font_xlarge = &lv_font_montserrat_48;

void vh_setup(void)
{
  // delay(5000);
  Serial.begin(115200);
  lcd.init(); // Initialize LovyanGFX
  lv_init();  // Initialize lvgl
  lv_fs_stdio_init();

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

void event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
  {
    vh_mock_data_toggle();
    LV_LOG_USER("Clicked");
  }
  else if (code == LV_EVENT_VALUE_CHANGED)
  {
    LV_LOG_USER("Toggled");
  }
}

void main_screen(void)
{
  apply_velohub_theme();
  lv_obj_t *p = vh_tiles_init(lv_scr_act());

  lv_obj_t *tile1 = vh_get_main_tile();

  // lv_obj_t *btn_m = lv_btn_create(tile1);

  // lv_obj_add_event_cb(btn_m, event_handler, LV_EVENT_ALL, NULL);
  // label_m = lv_label_create(btn_m);
  // lv_label_set_text(label_m, "Mock Data");

  // lv_obj_set_size(btn_m, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  // lv_obj_set_style_bg_color(btn_m, lv_color_hex(0x999999), 0);
  // lv_obj_align(btn_m, LV_ALIGN_BOTTOM_MID, 0, 0);

  // lv_obj_t *map_update_slider = lv_slider_create(tile1);
  // lv_obj_align(map_update_slider, LV_ALIGN_CENTER, 0, 25);
  // lv_slider_set_range(map_update_slider, 30, 10000);
  // lv_slider_set_value(map_update_slider, map_frequency, lv_anim_enable_t::LV_ANIM_OFF);
  // lv_obj_set_size(map_update_slider, 10, 300);
  // lv_obj_add_event_cb(map_update_slider, map_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lcd.startWrite();
  lcd.setAddrWindow(area->x1, area->y1, w, h);
  lcd.pushPixels((uint16_t *)&color_p->full, w * h, true);
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
    data->point.x = touchX;
    data->point.y = touchY;
    // Serial.printf("Touch (x,y): (%03d,%03d)\n",touchX,touchY );
  }
}

void display_task_code(void *parameter)
{
  Serial.println("display_task_code");
  vh_setup();
  for (;;)
  {
    vTaskDelay(2 / portTICK_PERIOD_MS);
    vh_loop();
  }
}