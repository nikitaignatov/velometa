#include "vh_container.hpp"

lv_obj_t *vh_create_container(lv_obj_t *parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t bg_hex)
{
    lv_obj_t *container = lv_obj_create(parent); /*Create a parent object on the current screen*/
    lv_obj_set_pos(container, x, y);
    lv_obj_set_size(container, width, height);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_bg_color(container, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_color(container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_pad_all(container, 0, 0);
    // lv_obj_set_style_(container, 0, 0);
    lv_obj_set_style_radius(container, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
    return container;
}
lv_obj_t *vh_create_container(lv_obj_t *parent, uint16_t width, uint16_t height)
{
    return vh_create_container(parent, 0, 0, width, height, 0x000000);
}