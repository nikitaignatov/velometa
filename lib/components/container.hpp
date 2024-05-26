#pragma once

#include "ui_base.hpp"

class Container : public UI_Base
{
public:
    Container(const Container &temp_obj) = delete;
    Container &operator=(const Container &temp_obj) = delete;

    Container(lv_obj_t *parent, uint16_t width, uint16_t height)
        : UI_Base()
    {
        lv_obj_t *container = lv_obj_create(parent); /*Create a parent object on the current screen*/
        lv_obj_set_pos(container, 0, 0);
        lv_obj_set_size(container, width, height);
        lv_obj_set_style_border_width(container, 0, 0);
        lv_obj_set_style_bg_color(container, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_color(container, lv_color_hex(0x000000), 0);
        lv_obj_set_style_pad_all(container, 0, 0);
        // lv_obj_set_style_(container, 0, 0);
        lv_obj_set_style_radius(container, 0, LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
        set_obj(container);
    }
};
