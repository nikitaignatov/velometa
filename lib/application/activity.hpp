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

typedef struct
{
    float *lat;
    float *lon;
    uint16_t *elevation;
    uint16_t *pressure;
    uint16_t *power;
    uint16_t *speed;
    uint8_t *hr;
    uint8_t *cadence;
    uint8_t *slope;
} ride_data_t;

class ActivityMinute
{
    uint16_t begin, end;
    metric_info_t power, hr, speed;
    ride_data_t *seconds;

public:
    void init(uint16_t being, uint16_t end);
    void add_measurement(raw_measurement_msg_t msg, uint32_t s);
};

class ActivityHour
{
    uint16_t begin, end;
    metric_info_t power, hr, speed;
    std::array<ActivityMinute, H_MINUTES> minutes;

public:
    void init(uint16_t being, uint16_t end);
    void add_measurement(raw_measurement_msg_t msg, int seconds);
};

class Activity
{
    uint16_t seconds = 0;
    uint64_t ts_start = 0;
    uint64_t ts_end = 0;
    metric_info_t power, hr, speed;
    std::array<ActivityHour, RIDE_HOURS_MAX> hours;

public:
    void init();
    void set_tick(uint16_t seconds);
    void add_measurement(raw_measurement_msg_t msg);
    metric_info_t get_hr();
    metric_info_t get_power();
    metric_info_t get_speed();
};

void activity_task_code(void *parameter);
