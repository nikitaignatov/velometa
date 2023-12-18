#pragma once

#include <lvgl.h>
#include "config.hpp"
#include "types.hpp"
#include "esp_log.h"
#include "task.h"
#include <algorithm>
#include <map>
#include "vh_display.hpp"

const uint16_t H_SECONDS = 3600;
const uint8_t RIDE_HOURS_MAX = 4;
const uint8_t H_MINUTES = 60;

class Activity
{
    uint16_t seconds = 0;
    uint64_t ts_start = 0;
    uint64_t ts_end = 0;

    // 5s,15s,30s,60s,90s,120s,300s,600s,900s
    std::map<measurement_t, std::array<window_counter_t, 10>> counters{
        {
            measurement_t::heartrate,
            std::array<window_counter_t, 10>{{
                {.duration = 5},
                {.duration = 15},
                {.duration = 30},
                {.duration = 60},
                {.duration = 90},
                {.duration = 120},
                {.duration = 300},
                {.duration = 600},
                {.duration = 900},
            }},
        },
        {
            measurement_t::power,
            std::array<window_counter_t, 10>{{
                {.duration = 5},
                {.duration = 15},
                {.duration = 30},
                {.duration = 60},
                {.duration = 90},
                {.duration = 120},
                {.duration = 300},
                {.duration = 600},
                {.duration = 900},
            }},
        },
        {
            measurement_t::speed,
            std::array<window_counter_t, 10>{{
                {.duration = 5},
                {.duration = 15},
                {.duration = 30},
                {.duration = 60},
                {.duration = 90},
                {.duration = 120},
                {.duration = 300},
                {.duration = 600},
                {.duration = 900},
            }},
        },
    };

public:
    void init();
    void tick();
    uint16_t get_tick();
    void set_tick(uint16_t seconds);
    void set_start(uint16_t seconds);
    void set_end(uint16_t seconds);
    void add_measurement(raw_measurement_msg_t msg);
    window_counter_t get_hr(uint16_t duration);
    window_counter_t get_hr();
    window_counter_t get_power();
    window_counter_t get_speed();
};

void activity_task_code(void *parameter);
