#pragma once

#include <map>
#include <lvgl.h>
#include <functional>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <string>
#include "config.hpp"
#include "types.hpp"
#include "fonts.hpp"
#include "zones.hpp"
#include "ui_base.hpp"

int zone_from_hr(float value);
int zone_from_power(float value);

class DistanceMap : public UI_Base
{
    lv_obj_t *canvas;
    lv_obj_t *obj;
    lv_obj_t *label;
    void event_cb(lv_event_t *e);

public:
    DistanceMap(const DistanceMap &temp_obj) = delete;
    DistanceMap &operator=(const DistanceMap &temp_obj) = delete;

    ~DistanceMap()
    {
        lv_obj_del_async(obj);
        lv_obj_del_async(label);
    }

    DistanceMap(lv_obj_t *parent, uint16_t event_id, const char *label_text, const char *value_text);
    void subscribe(uint16_t msg_id);
};
