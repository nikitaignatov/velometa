#ifndef _VH_LVGL_STATUS_BAR_HPP_
#define _VH_LVGL_STATUS_BAR_HPP_

#include "lvgl.h"
#include "vh_container.hpp"

static lv_style_t style_status_bar;

lv_obj_t *vh_create_status_bar(lv_obj_t *parent, int width)
{
    lv_style_init(&style_status_bar);
    lv_style_set_text_font(&style_status_bar, &lv_font_montserrat_12);
    lv_obj_t *status_bar = vh_create_container(parent, 0, 0, width, 20, 0x000000);

    lv_obj_t *lbl = lv_label_create(status_bar);
    lv_obj_add_style(lbl, &style_status_bar, 0);
    // lv_obj_add_style(lbl, &style_metric_label, 0);
    lv_label_set_recolor(lbl, true);
    lv_label_set_text(lbl, "0:00:00   145.2km   15.2km/h   16.1/km/h   21:00");
    lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);
    return status_bar;
}

#endif