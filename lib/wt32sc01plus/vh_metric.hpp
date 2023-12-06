#ifndef _VH_METRIC_HPP_
#define _VH_METRIC_HPP_

#include "lvgl.h"
#include "vh_container.hpp"

lv_obj_t *vh_show_metric(lv_obj_t *parent, std::string label, std::string value, lv_align_t align)
{
  lv_obj_t *lbl = lv_label_create(parent);
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

#endif