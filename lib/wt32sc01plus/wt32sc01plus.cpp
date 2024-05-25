#include "wt32sc01plus.hpp"
LGFX lcd;
Display *display;

/*** Setup screen resolution for LVGL ***/
static const uint16_t screen_width = 320;
static const uint16_t screen_height = 480;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screen_width * 10];

// Variables for touch x,y
#ifdef DRAW_ON_SCREEN
static int32_t x, y;
#endif

/*** Function declaration ***/
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void main_screen(void);

extern FATFS *fatfs;

void vh_setup(void)
{
  lcd.init(); // Initialize LovyanGFX
  lv_init();  // Initialize lvgl
  lv_fs_stdio_init();

  // Setting display to landscape
  if (lcd.width() < lcd.height())
    lcd.setRotation(2);
  lcd.setBrightness(128);

  /* LVGL : Setting up buffer to use for display */
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screen_width * 10);

  /*** LVGL : Setup & Initialize the display device driver ***/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screen_width;
  disp_drv.ver_res = screen_height;
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

  // Serial.print("stack::disaplay::");
  // Serial.println(uxTaskGetStackHighWaterMark(NULL));
}

void main_screen(void)
{
  apply_velohub_theme();
  // lv_obj_t *p = vh_tiles_init(lv_scr_act());
  auto screen = lv_scr_act();
  display = new Display(screen, screen_width, screen_height);
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
  ESP_LOGI("display_task_code", "start");
  vh_setup();
  ESP_LOGI("display_task_code", "vh_setup complete");
  for (;;)
  {
    vTaskDelay(2 / portTICK_PERIOD_MS);
    if (xSemaphoreTake(vh_display_semaphore, (TickType_t)10) == pdTRUE)
    {
      vh_loop();
      xSemaphoreGive(vh_display_semaphore);
    }
    else
    {
      ESP_LOGD("display_task_code", "failed to take semaphor");
    }
  }
}