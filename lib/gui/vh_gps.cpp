#include "vh_gps.hpp"
#include "TinyGPSPlus.h"
lv_obj_t *tile;
lv_obj_t *date;
lv_obj_t *time_;
lv_obj_t *hdop;
lv_obj_t *speed_;
lv_obj_t *alt;
lv_obj_t *lat;
lv_obj_t *lon;
lv_obj_t *age;
lv_obj_t *sat;
lv_obj_t *fix;
lv_obj_t *zoom;
lv_obj_t *tile_path;

static void label_event_cb(lv_event_t *e)
{
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    // const char *formatter = (char *)lv_msg_get_user_data(m);
    const gps_data_t *v = (gps_data_t *)lv_msg_get_payload(m);

    int zoom = 15;
    if (m->id == 2222)
    {
        lv_label_set_text(hdop, fmt::format("HDOP: {}", v->hdop).c_str());
        lv_label_set_text(date, fmt::format("D: {}", v->tick_ms).c_str());
        lv_label_set_text(time_, fmt::format("T: {}", v->tick_ms).c_str());
        lv_label_set_text(lat, fmt::format("LAT: {:.5f}", v->lat).c_str());
        lv_label_set_text(lon, fmt::format("LON: {:.5f}", v->lon).c_str());
        lv_label_set_text(speed_, fmt::format("SPD: {:.2f}", v->speed).c_str());
        lv_label_set_text(alt, fmt::format("ALT: {}", v->height).c_str());
        lv_label_set_text(age, fmt::format("AGE: {}", v->age).c_str());
        lv_label_set_text(sat, fmt::format("SAT: {}", v->satelites).c_str());
        lv_label_set_text(fix, fmt::format("FIX: {}", v->has_fix).c_str());
    }
}

lv_obj_t *vh_gps_tile_create(lv_obj_t *parent)
{
    tile = vh_create_container(parent, 320, 480);
    lv_obj_center(tile);
    lv_msg_subsribe_obj(2222, tile, 0);
    lv_obj_add_event_cb(tile, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);

    speed_ = lv_label_create(tile);

    alt = lv_label_create(tile);
    lat = lv_label_create(tile);
    lon = lv_label_create(tile);
    date = lv_label_create(tile);
    time_ = lv_label_create(tile);
    hdop = lv_label_create(tile);
    age = lv_label_create(tile);
    sat = lv_label_create(tile);
    fix = lv_label_create(tile);
    tile_path = lv_label_create(tile);

    lv_obj_align(date, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_align(time_, LV_ALIGN_TOP_LEFT, 0, 20);
    lv_obj_align(lat, LV_ALIGN_TOP_LEFT, 0, 40);
    lv_obj_align(lon, LV_ALIGN_TOP_LEFT, 0, 60);
    lv_obj_align(speed_, LV_ALIGN_TOP_LEFT, 0, 80);
    lv_obj_align(alt, LV_ALIGN_TOP_LEFT, 0, 100);
    lv_obj_align(hdop, LV_ALIGN_TOP_LEFT, 0, 120);
    lv_obj_align(age, LV_ALIGN_TOP_LEFT, 0, 140);
    lv_obj_align(sat, LV_ALIGN_TOP_LEFT, 0, 160);
    lv_obj_align(fix, LV_ALIGN_TOP_LEFT, 0, 180);
    lv_obj_align(tile_path, LV_ALIGN_TOP_LEFT, 0, 200);

    return tile;
}
