#include "vh_gps.hpp"

static const char *TAG = "vh_gps";

static lv_obj_t *tile;
static lv_obj_t *time_;
static lv_obj_t *hdop;
static lv_obj_t *speed_;
static lv_obj_t *alt;
static lv_obj_t *lat;
static lv_obj_t *lon;
static lv_obj_t *age;
static lv_obj_t *sat;
static lv_obj_t *fix;
static lv_obj_t *zoom;
static lv_obj_t *tile_path;

static void label_event_cb(lv_event_t *e)
{
    auto m = lv_event_get_msg(e);
    if (!m)
    {
        ESP_LOGE(TAG, "label_event_cb::msg::empty");
        return;
    }
    if (m->id != MSG_NEW_GPS)
    {
        ESP_LOGE(TAG, "msg->id invlaid=%d, exected=%d", m->id, MSG_NEW_GPS);
        return;
    }
    auto p = lv_msg_get_payload(m);
    if (!p)
    {
        ESP_LOGE(TAG, "msg payload is missing");
        return;
    }
    auto v = (gps_data_t *)p;
    if (v && v->tick_ms > 0)
    {
        int zoom = 15;
        // Serial.println("update gps");
        lv_label_set_text(hdop, fmt::format("HDOP: {}", v->hdop).c_str());
        lv_label_set_text(time_, fmt::format("T   : {}", v->tick_ms).c_str());
        lv_label_set_text(lat, fmt::format("LAT : {:.5f}", v->lat).c_str());
        lv_label_set_text(lon, fmt::format("LON : {:.5f}", v->lon).c_str());
        vTaskDelay(1 / portTICK_PERIOD_MS);
        lv_label_set_text(speed_, fmt::format("SPD : {:.2f}", v->speed).c_str());
        lv_label_set_text(alt, fmt::format("ALT : {}", v->height).c_str());
        lv_label_set_text(age, fmt::format("AGE : {}", v->age).c_str());
        lv_label_set_text(sat, fmt::format("SAT : {}", v->satelites).c_str());
        lv_label_set_text(fix, fmt::format(v->has_fix ? "FIX : #009900 {}#" : "FIX : #990000 {}#", v->has_fix).c_str());
    }
    else
    {
        ESP_LOGE(TAG, "gps data is invalid or outdated.");
    }
}

lv_obj_t *vh_gps_tile_create(lv_obj_t *parent)
{
    auto container = new Container(parent, 320, 450);
    tile = container->get_obj();
    lv_obj_center(tile);
    lv_msg_subsribe_obj(MSG_NEW_GPS, tile, 0);
    lv_obj_add_event_cb(tile, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);

    speed_ = lv_label_create(tile);

    alt = lv_label_create(tile);
    lat = lv_label_create(tile);
    lon = lv_label_create(tile);
    time_ = lv_label_create(tile);
    hdop = lv_label_create(tile);
    age = lv_label_create(tile);
    sat = lv_label_create(tile);
    fix = lv_label_create(tile);
    tile_path = lv_label_create(tile);

    lv_label_set_recolor(fix, true);

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
