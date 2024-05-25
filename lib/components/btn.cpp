#include "btn.hpp"

Btn::Btn(lv_obj_t *parent, bool next) : UI_Base()
{
    int offset = 15;
    int size = 50;
    auto color = lv_color_hex(0xfafafa);
    auto text_color = lv_color_hex(0x000000);
    btn = lv_btn_create(parent);
    label = lv_label_create(btn);
    lv_obj_set_style_text_color(label, text_color, 0);
    lv_obj_set_style_bg_color(label, color, 0);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_size(btn, size, size);
    if (next)
    {
        lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -offset, -offset);
    }
    else
    {
        lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, offset, -offset);
    }
    lv_label_set_text(label, this->text);
    lv_obj_center(label);
    set_obj(btn);
}

void Btn::on_press(vm_lv_event_cb_t cb)
{
    this->set_callback(cb, LV_EVENT_PRESSING);
}

void Btn::set_text(const char *txt)
{
    this->text = txt;
    lv_label_set_text(this->label, txt);
}