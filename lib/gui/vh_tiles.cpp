#include "vh_tiles.hpp"

int active = 0;

lv_obj_t *tv;
lv_obj_t *tile1;
lv_obj_t *scan_tile;
lv_obj_t *map_tile;

lv_obj_t *vh_get_main_tile() { return tile1; }
lv_obj_t *vh_get_settings_tile() { return scan_tile; }
lv_obj_t *vh_get_map_tile() { return map_tile; }

static void prev_tile_cb(lv_event_t *e)
{
    if (active > 0)
    {
        active--;
    }
    else
    {
        active = lv_obj_get_child_cnt(tv) - 1;
    }
    lv_obj_set_tile_id(tv, active, 0, LV_ANIM_OFF);
};

static void next_tile_cb(lv_event_t *e)
{
    if (active < lv_obj_get_child_cnt(tv) - 1)
    {
        active++;
    }
    else
    {
        active = 0;
    }
    lv_obj_set_tile_id(tv, active, 0, LV_ANIM_OFF);
};

lv_obj_t *create_btn(lv_obj_t *parent, bool next)
{
    int offset = 15;
    int size = 50;
    lv_color_t color = lv_color_hex(0x090909);
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_t *label = lv_label_create(btn);
    lv_obj_set_style_bg_color(label, color, 0);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_size(btn, size, size);
    if (next)
    {
        lv_obj_add_event_cb(btn, next_tile_cb, LV_EVENT_PRESSING, NULL);
        lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -offset, -offset);
    }
    else
    {
        lv_obj_add_event_cb(btn, prev_tile_cb, LV_EVENT_PRESSING, NULL);
        lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, offset, -offset);
    }
    lv_label_set_text(label, next ? ">" : "<");
    lv_obj_center(label);
    return btn;
}

lv_obj_t *vh_tiles_init(lv_obj_t *parent)
{
    tv = lv_tileview_create(parent);
    tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_RIGHT);
    scan_tile = lv_tileview_add_tile(tv, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    map_tile = lv_tileview_add_tile(tv, 2, 0, LV_DIR_LEFT);
    lv_obj_t *tile4 = lv_tileview_add_tile(tv, 3, 0, LV_DIR_LEFT);
    lv_obj_t *tile5 = lv_tileview_add_tile(tv, 4, 0, LV_DIR_LEFT);
    lv_obj_t *gps_tile = lv_tileview_add_tile(tv, 5, 0, LV_DIR_LEFT);
    lv_obj_t *btn_next = create_btn(parent, true);
    lv_obj_t *btn_prev = create_btn(parent, false);

    vh_create_brightness_slider(scan_tile);
    vh_map_init(map_tile);

    auto *p = vh_gps_tile_create(gps_tile);

    return tv;
}