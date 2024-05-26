#include "status_bar.hpp"

static lv_style_t style_status_bar;

StatusBar::StatusBar(lv_obj_t *parent, uint16_t width)
{
    lv_style_init(&style_status_bar);
    lv_style_set_text_font(&style_status_bar, &lv_font_montserrat_12);
    auto container = new Container(parent, width, 20);
    auto status_bar = container->get_obj();
    set_obj(status_bar);
    lv_obj_align(status_bar, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t *lbl = lv_label_create(status_bar);
    lv_obj_add_style(lbl, &style_status_bar, 0);
    // lv_obj_add_style(lbl, &style_metric_label, 0);
    lv_label_set_recolor(lbl, true);
    // lv_label_set_text(lbl, "0:00:00   145.2km   15.2km/h   16.1/km/h   21:00");
    lv_label_set_text(lbl, "0:00:00 #000000 HR# #999999 GPS# #666666 PWR# #666666 SPD#");
    lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);
}