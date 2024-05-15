#include "vh_tiles.hpp"

int active = 0;

lv_obj_t *tv;
lv_obj_t *dashboard_tile;
lv_obj_t *scan_tile;
lv_obj_t *cda_tile;
lv_obj_t *map_tile;

lv_obj_t *vh_get_main_tile() { return dashboard_tile; }
lv_obj_t *vh_get_settings_tile() { return scan_tile; }
lv_obj_t *vh_get_cda_tile() { return cda_tile; }
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
    auto color = lv_color_hex(0x090909);
    auto btn = lv_btn_create(parent);
    auto label = lv_label_create(btn);
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
    lv_obj_set_size(tv, 320, 450);
    lv_obj_align(tv, LV_ALIGN_TOP_MID, 0, 0);
    vh_create_status_bar(lv_scr_act(), 320);

    dashboard_tile = lv_tileview_add_tile(tv, 0, 0, LV_DIR_NONE);
    scan_tile = lv_tileview_add_tile(tv, 1, 0, LV_DIR_NONE);
    cda_tile = lv_tileview_add_tile(tv, 2, 0, LV_DIR_NONE);
    auto gps_tile = lv_tileview_add_tile(tv, 3, 0, LV_DIR_NONE);
    // map_tile = lv_tileview_add_tile(tv, 3, 0, LV_DIR_NONE);
    // auto tile4 = lv_tileview_add_tile(tv, 4, 0, LV_DIR_NONE);
    // auto tile5 = lv_tileview_add_tile(tv, 5, 0, LV_DIR_NONE);
    // auto tile6 = lv_tileview_add_tile(tv, 6, 0, LV_DIR_NONE);
    auto btn_next = create_btn(parent, true);
    auto btn_prev = create_btn(parent, false);

    // vh_create_activity_hr_stats(tile5);
    vh_create_brightness_slider(scan_tile);
    vm_show_cda_panel(cda_tile);
    vh_map_init(map_tile);
    vh_create_dashboard(dashboard_tile);
    // vh_create_mock_btn(parent);
    // vh_create_histogram(tile6, 320, 350, 5);

    auto p = vh_gps_tile_create(gps_tile);

    return tv;
}
