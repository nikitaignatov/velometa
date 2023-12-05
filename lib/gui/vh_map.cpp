#include "vh_map.hpp"

#define WINDOW_WIDTH 320
#define WINDOW_HEIGH 128
#define TILE_WIDTH 256
#define TILE_WIDTH 256
#define TILE_HEIGHT TILE_WIDTH

lv_obj_t *map_window;
lv_obj_t *map;

void vh_map_init(lv_obj_t *parent)
{
    map_window = vh_create_container(parent, WINDOW_WIDTH, WINDOW_HEIGH);
    lv_obj_align(map_window, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    map = lv_img_create(map_window);
    lv_img_set_antialias(map, false);
    lv_img_set_src(map, "S:/70020_40974.bin");
    lv_img_set_antialias(map, false);
    lv_obj_align(map, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_set_size(map, TILE_WIDTH, WINDOW_HEIGH);
    // lv_img_set_offset_x(map, -10);
    // lv_img_set_offset_y(map, -50);
    // lv_obj_set_pos(map, 0, 480 - 128);

    lv_obj_t *map2 = lv_img_create(map_window);
    lv_img_set_antialias(map2, false);
    lv_img_set_src(map2, "S:/70021_40974.bin");
    lv_obj_set_size(map2, WINDOW_WIDTH - TILE_WIDTH, WINDOW_HEIGH);
    lv_obj_align_to(map2, map, LV_ALIGN_TOP_LEFT, TILE_WIDTH, 0);

    lv_obj_t *led1 = lv_led_create(map_window);
    lv_obj_center(led1);
    lv_obj_set_size(led1, 10, 10);
    lv_led_on(led1);
}

static void vh_map_update(int zoom, int x, int y)
{
    auto path = fmt::format("S:/{}/{}/{}.bin", zoom, x, y);
    lv_img_set_src(map, path.c_str());
}