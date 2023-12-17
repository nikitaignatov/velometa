#pragma once

#include <deque>
#include <stdint.h>
#include <array>

struct window_counter_t
{
    std::deque<uint16_t> min_deque = {};
    std::deque<uint16_t> max_deque = {};

    inline void add_min(uint16_t value)
    {
        while (max_deque.size() > 0 && value > max_deque.back())
        {
            max_deque.pop_back();
        }
        max_deque.push_back(value);
    }

    inline void remove_last_min(uint16_t last)
    {
        if (min_deque.front() == last)
        {
            min_deque.pop_front();
        }
    }

    inline void add_max(uint16_t value)
    {
        while (min_deque.size() > 0 && value < min_deque.back())
        {
            min_deque.pop_back();
        }
        min_deque.push_back(value);
    }

    inline void remove_last_max(uint16_t last)
    {
        if (max_deque.front() == last)
        {
            max_deque.pop_front();
        }
    }

public:
    uint16_t duration;
    uint16_t window_end;
    uint16_t window_start;
    uint16_t last;
    uint16_t min = std::numeric_limits<uint16_t>::max();
    uint16_t max = std::numeric_limits<uint16_t>::min();
    uint16_t avg;
    uint16_t sum;
    uint16_t count;

    void add(window_counter_t *self, uint16_t value, uint16_t *telemetry)
    {
        self->last = value;
        self->window_end++;
        self->sum += value;
        add_min(value);
        add_max(value);

        if (self->count < self->duration)
        {
            self->count++;
        }
        else
        {
            auto last = telemetry[self->window_start];
            self->sum -= last;
            remove_last_min(last);
            remove_last_max(last);
        }

        self->max = max_deque.front();
        self->min = min_deque.front();
        self->avg = self->sum / (std::max<uint16_t>(1, self->count));
        self->window_start = self->window_end - self->count;
    };
};