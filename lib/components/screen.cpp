#include "screen.hpp"

Display::Display(lv_obj_t *parent, uint16_t w, uint16_t h)
{
    this->_parent = parent;
    this->set_width(w);
    this->set_height(h);
    this->tv = lv_tileview_create(parent);

    auto dashboard_tile = lv_tileview_add_tile(tv, 0, 0, LV_DIR_NONE);
    auto scan_tile = lv_tileview_add_tile(tv, 1, 0, LV_DIR_NONE);
    auto cda_tile = lv_tileview_add_tile(tv, 2, 0, LV_DIR_NONE);
    auto gps_tile = lv_tileview_add_tile(tv, 3, 0, LV_DIR_NONE);

    lv_obj_set_size(tv, this->width, height);
    lv_obj_align(tv, LV_ALIGN_TOP_MID, 0, 0);
    status = new StatusBar(lv_scr_act(), this->width);

    btn_next = new Btn(parent, true);
    btn_next->set_text(">");
    btn_prev = new Btn(parent, false);
    btn_prev->set_text("<");

    vh_create_brightness_slider(scan_tile);
    vm_show_cda_panel(cda_tile);
    vh_create_dashboard(dashboard_tile);

    auto p = vh_gps_tile_create(gps_tile);
    pages = lv_obj_get_child_cnt(tv);

    btn_next->on_press([&](auto *e)
                       { next_tile(); });

    btn_prev->on_press([&](auto *e)
                       { prev_tile(); });
}

void Display::set_width(uint16_t w)
{
    if (w > 0 && w < 1920)
    {
        width = w;
    }
}

void Display::set_height(uint16_t h)
{
    if (h > 0 && h < 1920)
    {
        height = h;
    }
}

void Display::next_tile()
{
    if (this->active < this->pages - 1)
    {
        this->active++;
    }
    else
    {
        this->active = 0;
    }
    if (this->tv != nullptr)
    {
        lv_obj_set_tile_id(this->tv, this->active, 0, LV_ANIM_OFF);
    }
}

void Display::prev_tile()
{
    if (this->active > 0)
    {
        this->active--;
    }
    else
    {
        this->active = this->pages - 1;
    }
    if (this->tv != nullptr)
    {
        lv_obj_set_tile_id(this->tv, this->active, 0, LV_ANIM_OFF);
    }
}