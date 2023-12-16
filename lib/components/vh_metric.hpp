#pragma once

#include <lvgl.h>
#include <functional>
#include <fmt/core.h>
#include <Arduino.h>
#include <fmt/ranges.h>
#include <string>
#include "config.hpp"
#include "types.hpp"
#include "vh_container.hpp"
#include "vh_fonts.hpp"
#include "zones.hpp"

int zone_from_hr(float value);
int zone_from_power(float value);

class MetricW
{
    uint16_t width = 158, height = 80, x = 0, y = 0;
    uint32_t subscribed_msg_id = 0;
    // metric_info_t *metric;
    lv_obj_t *container = 0;
    lv_obj_t *value = 0;
    lv_obj_t *min = 0;
    lv_obj_t *max = 0;
    lv_obj_t *avg = 0;
    lv_obj_t *zone = 0;
    lv_obj_t *label = 0;
    bool opposite = false;
    std::function<int(float)> zone_converter;

public:
    ~MetricW()
    {
        lv_obj_del(this->container);
    }
    void init(lv_obj_t *parent);
    void subscribe(uint32_t msg_id);
    void set_label(std::string value);
    void set_opposite(bool value);
    void set_zone_converter(std::function<int(float)> converter);
    void update(window_counter_t *metric);
    lv_obj_t *get_object();
};