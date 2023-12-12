#pragma once

#include "types.hpp"
#include "metric.hpp"
#include <algorithm>

const uint16_t H_SECONDS = 3600;
const uint8_t RIDE_HOURS_MAX = 4;
const uint8_t H_MINUTES = 60;

typedef struct
{
    uint16_t duration;
    float last;
    float min;
    float max;
    float avg;
    float sum;
    float count;
    float std;
    float var;
} metric_s_t;

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

static ride_data_t *telemetry;

static metric_s_t new_reading(metric_s_t total, raw_measurement_msg_t measurement)
{
    auto value = measurement.value;
    total.last = value;
    total.sum += value;
    total.count++;
    total.min = std::min(total.min, (float)value);
    total.max = std::max(total.max, (float)value);
    total.avg = total.sum / total.count;
    return total;
}

class RideMinute
{
    uint16_t begin, end;
    metric_s_t power, hr, speed;
    ride_data_t *seconds;

public:
    void init(ride_data_t *seconds, uint16_t being, uint16_t end)
    {
        this->seconds = seconds;
        this->begin = begin;
        this->end = end;
    }

    void add_measurement(raw_measurement_msg_t msg, uint32_t s)
    {
        if (s >= begin && s <= end)
        {
            switch (msg.measurement)
            {
            case measurement_t::heartrate:
                telemetry->hr[s] = msg.value;
                hr = new_reading(hr, msg);
                return;
            case measurement_t::power:
                telemetry->power[s] = msg.value;
                power = new_reading(power, msg);
                return;
            case measurement_t::speed:
                telemetry->speed[s] = msg.value;
                speed = new_reading(speed, msg);
                return;
            default:
                break;
            }
        }
    }
};

class RideHour
{
    uint16_t begin, end;
    metric_s_t power, hr, speed;
    std::array<RideMinute, H_MINUTES> minutes;

public:
    void init(uint16_t being, uint16_t end)
    {
        this->begin = begin;
        this->end = end;
        int i = 0;
        for (auto minute : minutes)
        {
            minute.init(telemetry + i, i, i + H_MINUTES);
            i = i + H_MINUTES;
        }
    }

    void add_measurement(raw_measurement_msg_t msg, int seconds)
    {
        auto h = seconds / H_SECONDS;
        auto m = h % 60;
        auto minute = minutes[m];
        minute.add_measurement(msg, seconds);
    }
};

class Ride
{
    uint16_t seconds = 0;
    uint64_t ts_start = 0;
    uint64_t ts_end = 0;
    metric_s_t power, hr, speed;
    std::array<RideHour, RIDE_HOURS_MAX> hours;

public:
    void init()
    {
        telemetry = (ride_data_t *)ps_malloc(RIDE_HOURS_MAX * H_SECONDS * sizeof(ride_data_t));
        for (auto hour : hours)
        {
            hour.init(0, H_SECONDS);
        }
    }
    void set_tick(uint16_t seconds)
    {
        this->seconds = seconds;
    }
    void add_measurement(raw_measurement_msg_t msg)
    {
        auto index = msg.ts / H_SECONDS;
        auto hour = hours[index];
        hour.add_measurement(msg, seconds);
    }
};
