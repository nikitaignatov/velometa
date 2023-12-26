#pragma once

#include <lvgl.h>
#include "config.hpp"
#include "types.hpp"
#include "zones.hpp"
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

    uint16_t hr_zone_count = 0;
    std::map<uint16_t, int> hr_zone_hist{
        {1, 0},
        {2, 0},
        {3, 0},
        {4, 0},
        {5, 0},
    };
    uint16_t power_zone_count = 0;
    std::map<uint16_t, int> power_zone_hist{
        {1, 0},
        {2, 0},
        {3, 0},
        {4, 0},
        {5, 0},
        {6, 0},
        {7, 0},
    };

    // 5s,15s,30s,60s,90s,120s,300s,600s,900s
    std::map<measurement_t, std::array<window_counter_t, 16>> counters{
        {
            measurement_t::heartrate,
            std::array<window_counter_t, 16>{{
                {.duration = 1},
                {.duration = 3},
                {.duration = 5},
                {.duration = 10},
                {.duration = 15},
                {.duration = 30},
                {.duration = 60},
                {.duration = 90},
                {.duration = 120},
                {.duration = 300},
                {.duration = 600},
                {.duration = 900},
                {.duration = 1200},
                {.duration = 1800},
                {.duration = 3600},
                {.duration = 2 * 3600},
            }},
        },
        {
            measurement_t::power,
            std::array<window_counter_t, 16>{{
                {.duration = 1},
                {.duration = 3},
                {.duration = 5},
                {.duration = 10},
                {.duration = 15},
                {.duration = 30},
                {.duration = 60},
                {.duration = 90},
                {.duration = 120},
                {.duration = 300},
                {.duration = 600},
                {.duration = 900},
                {.duration = 1200},
                {.duration = 1800},
                {.duration = 3600},
                {.duration = 2 * 3600},
            }},
        },
        {
            measurement_t::speed,
            std::array<window_counter_t, 16>{{
                {.duration = 1},
                {.duration = 3},
                {.duration = 5},
                {.duration = 10},
                {.duration = 15},
                {.duration = 30},
                {.duration = 60},
                {.duration = 90},
                {.duration = 120},
                {.duration = 300},
                {.duration = 600},
                {.duration = 900},
                {.duration = 1200},
                {.duration = 1800},
                {.duration = 3600},
                {.duration = 2 * 3600},
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
    window_counter_t get_power(uint16_t duration);
    float get_tl()
    {
        auto power = counters[measurement_t::power][15].get_avg_w();
        auto r = static_cast<float>(power) / FTP ;
        auto p = (seconds * counters[measurement_t::power][15].get_avg_w() * r) / (FTP * 3600) * 100;
        return p;
    };
    uint32_t get_xpower()
    {
        return counters[measurement_t::power][15].get_avg_w();
    };
    window_counter_t get_speed();
    std::map<uint16_t, uint16_t> get_hr_zone_hist();
    std::map<uint16_t, uint16_t> get_power_zone_hist();
};

void activity_task_code(void *parameter);
Activity *current_activity();