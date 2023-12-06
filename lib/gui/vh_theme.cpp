#include "vh_theme.hpp"


static lv_style_t style_bg;

static void velohub_theme_callback(lv_theme_t *th, lv_obj_t *obj)
{
  LV_UNUSED(th);
  lv_obj_add_style(obj, &style_bg, 0);
}

void apply_velohub_theme(void)
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
