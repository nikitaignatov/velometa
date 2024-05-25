#pragma once

#include <map>
#include <lvgl.h>
#include <functional>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <string>
#include "config.hpp"
#include "types.hpp"
#include "vh_fonts.hpp"
#include "zones.hpp"
#include "ui_base.hpp"

int zone_from_hr(float value);
int zone_from_power(float value);

class PositionSlider : public UI_Base
{
    lv_obj_t *obj;
    lv_obj_t *label;
    lv_obj_t *value;
    void event_cb(lv_event_t *e);

public:
    PositionSlider(const PositionSlider &temp_obj) = delete;
    PositionSlider &operator=(const PositionSlider &temp_obj) = delete;

    ~PositionSlider()
    {
        lv_obj_del_async(obj);
        lv_obj_del_async(label);
        lv_obj_del_async(value);
    }

    PositionSlider(lv_obj_t *parent, measurement_t event_id, const char *label_text, const char *value_text);
    void subscribe(measurement_t msg_id);
};