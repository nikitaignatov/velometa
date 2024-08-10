#include "distance_map.hpp"
#include "Arduino.h"

#define CANVAS_PIXEL 8
#define CANVAS_WIDTH CANVAS_PIXEL *CANVAS_PIXEL
#define CANVAS_HEIGHT CANVAS_PIXEL *CANVAS_PIXEL
int16_t payload[64];

void DistanceMap::event_cb(lv_event_t *e)
{
    auto m = lv_event_get_msg(e);
    auto value_text = (const char *)lv_event_get_user_data(e);
    if (!m)
    {
        ESP_LOGE("TAG", "label_event_cb::msg::empty");
        return;
    }

    auto p = lv_msg_get_payload(m);
    if (!p)
    {
        ESP_LOGE("TAG", "msg payload is missing");
        return;
    }
    memcpy(payload, p, 64);
    if (payload)
    {
        /*Create hole on the canvas*/
        auto imageWidth = 8;
        int yy = 0;

        for (int y = 0; y <= imageWidth * (imageWidth - 1); y += imageWidth)
        {
            for (int x = imageWidth - 1; x >= 0; x--)
            {
                auto p = payload[x + y];
                auto q = map(p, 0, 2000, 0, 255);
                auto c =  p > 255 ? 255 : 0;

                Serial.print(q);
                Serial.print(":");
                Serial.print(c);
                Serial.print("\t");
                // lv_canvas_set_px_color(canvas, x, yy, lv_color_make(c, 0, c / 2));
            }
            yy++;
            Serial.print("\n");
        }

        Serial.println();
    }
    else
    {
        ESP_LOGE("TAG", "gps data is invalid or outdated.");
    }
}

void DistanceMap::subscribe(uint16_t msg_id)
{
    if (obj != nullptr)
    {
        lv_msg_subsribe_obj(msg_id, canvas, 0);
    }
}

DistanceMap::DistanceMap(lv_obj_t *parent, uint16_t event_id, const char *label_text, const char *value_text)
    : UI_Base()
{
    if (parent == nullptr)
    {
        ESP_LOGW("TAG", "distance_event_cb::DistanceMap::r");
        return;
    }

    canvas = lv_canvas_create(lv_scr_act());
    /*Create a buffer for the canvas*/
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

    /*Create a canvas and initialize its palette*/

    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_canvas_fill_bg(canvas, lv_color_make(0, 0, 0), LV_OPA_COVER);
    obj = canvas;
    set_obj(canvas);

    set_callback([&](auto *e)
                 { event_cb(e); }, LV_EVENT_MSG_RECEIVED);
    subscribe(event_id);
}