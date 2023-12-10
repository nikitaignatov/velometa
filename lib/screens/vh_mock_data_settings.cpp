#include "vh_mock_data_settings.hpp"

static const char *TAG = "vh_mock_data_settings";

static lv_obj_t *tile;

static void label_event_cb(lv_event_t *e)
{
}

lv_obj_t *vh_mock_data_settings(lv_obj_t *parent)
{
    tile = vh_create_container(parent, 320, 480);
    lv_obj_center(tile);

    return tile;
}
