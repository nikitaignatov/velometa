#pragma once

#include <lvgl.h>
#include "config.hpp"
#include "types.hpp"
#include "esp_log.h"
#include <algorithm>
#include "vh_display.hpp"

const uint16_t H_SECONDS = 3600;
const uint8_t RIDE_HOURS_MAX = 4;
const uint8_t H_MINUTES = 60;

struct window_counter_t
{
public:
    uint16_t duration;
    uint16_t window_end;
    uint16_t window_start;
    uint16_t min;
    uint16_t max;
    uint16_t avg;
    uint16_t sum;
    uint16_t count;
    void add(window_counter_t *self, raw_measurement_msg_t msg, uint16_t old)
    {
        self->window_end++;
        if (self->count < self->duration)
        {
            self->count++;
        }
        else
        {
            self->sum -= old;
        }

        self->sum += msg.value;
        self->avg = self->sum / (std::max<uint16_t>(1, self->count));
    };
};

class Activity
{
    uint16_t seconds = 0;
    uint64_t ts_start = 0;
    uint64_t ts_end = 0;
    metric_info_t power, hr, speed;

    // 5s,15s,30s,60s,90s,120s,300s,600s,900s
    std::array<window_counter_t, 10> counters{{
        {.duration = 5},
        {.duration = 15},
        {.duration = 30},
        {.duration = 60},
        {.duration = 90},
        {.duration = 120},
        {.duration = 300},
        {.duration = 600},
        {.duration = 900},
    }};

public:
    void init();
    void set_tick(uint16_t seconds);
    void add_measurement(raw_measurement_msg_t msg);
    metric_info_t get_hr();
    metric_info_t get_power();
    metric_info_t get_speed();
};

void activity_task_code(void *parameter);
