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


void setup(void)
{

  Serial.begin(115200); /* prepare for possible serial debug */
  lcd.init();           // Initialize LovyanGFX
  lv_init();            // Initialize lvgl

  // Setting display to landscape
  if (lcd.width() < lcd.height())
    lcd.setRotation(0);
  lcd.setBrightness(255);


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
  // lv_button_demo();

    static lv_style_t my_style;
    lv_style_init(&my_style);
  // lv_style_set_text_font(&my_style, LV_STATE_DEFAULT, &lv_font_montserrat_28);  /*Set a larger font*/

  lv_example_tileview_1();
}

void lv_example_get_started_1(void)
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "VeloHUB");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xfeed62), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void lv_example_tileview_1(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xfeed62), LV_PART_MAIN);
    lv_obj_t *tv = lv_tileview_create(lv_scr_act());

    /*Tile1: just a label*/
    lv_obj_t * tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM);
    lv_obj_t * label = lv_label_create(tile1);
    lv_label_set_recolor(label, true);    
    lv_label_set_text(label, "#feed62 Velo##feff62 HUB#");
    lv_obj_center(label); 





    /*Tile2: a button*/
    lv_obj_t * tile2 = lv_tileview_add_tile(tv, 0, 1, LV_DIR_TOP | LV_DIR_RIGHT);

    lv_obj_t * hr = lv_label_create(tile1);
    lv_label_set_recolor(hr, true);    
    lv_label_set_text(hr, "#ffffff HR:# #feff62 82#");
    lv_obj_center(hr);
    lv_obj_t * btn = lv_btn_create(tile2);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Scroll up or right");

    lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn);

    /*Tile3: a list*/
    lv_obj_t * tile3 =  lv_tileview_add_tile(tv, 1, 1, LV_DIR_LEFT);
    lv_obj_t * list = lv_list_create(tile3);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));

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

}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);

  // if (false&&lcd.getTouch(&x, &y))
  // {
  //   lcd.fillRect(x - 2, y - 2, 2, 2, TFT_RED);
  //   lcd.setCursor(0, 0);
  //   lcd.printf("Touch w:(%03d,%03d)", x, y);
  // }
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

/* Counter button event handler */
static void counter_event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED)
  {
    static uint8_t cnt = 0;
    cnt++;

    /*Get the first child of the button which is the label and change its text*/
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "Button: %d", cnt);
    LV_LOG_USER("Clicked");
    Serial.println("Clicked");
  }
}

/* Toggle button event handler */
static void toggle_event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_VALUE_CHANGED)
  {
    LV_LOG_USER("Toggled");
    Serial.println("Toggled");
  }
}

void lv_button_demo(void)
{
  lv_obj_t *label;

  // Button with counter
  lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn1, counter_event_handler, LV_EVENT_ALL, NULL);

  lv_obj_set_pos(btn1, 100, 100); /*Set its position*/
  lv_obj_set_size(btn1, 120, 50); /*Set its size*/

  label = lv_label_create(btn1);
  lv_label_set_text(label, "Start Batch");
  lv_obj_center(label);

  // Toggle button
  lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn2, toggle_event_handler, LV_EVENT_ALL, NULL);
  lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_pos(btn2, 250, 100); /*Set its position*/
  lv_obj_set_size(btn2, 120, 50); /*Set its size*/

  label = lv_label_create(btn2);
  lv_label_set_text(label, "Toggle Button");
  lv_obj_center(label);
}
