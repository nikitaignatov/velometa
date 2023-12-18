#pragma once

#include <deque>
#include <stdint.h>
#include <array>
#include <functional>

struct window_counter_t
{
    std::deque<uint32_t> min_deque = {};
    std::deque<uint32_t> max_deque = {};

private:
    inline void add_deque_min(std::deque<uint32_t> *deque, uint32_t value) { add_deque(deque, value, std::less{}); }
    inline void add_deque_max(std::deque<uint32_t> *deque, uint32_t value) { add_deque(deque, value, std::greater{}); }

    template <typename Comparator>
    inline void add_deque(std::deque<uint32_t> *deque, uint32_t value, Comparator compare)
    {
        while (deque->size() > 0 && compare(value, deque->back()))
        {
            deque->pop_back();
        }
        if (deque->size() == 0 || value != deque->back())
        {
            deque->push_back(value);
        }
    }

    inline void remove_last(std::deque<uint32_t> *deque, uint32_t last)
    {
        if (deque->size() > 1 && deque->front() == last)
        {
            deque->pop_front();
        }
    }

public:
    uint32_t duration = 0;
    uint32_t last = 0;
    uint32_t min = std::numeric_limits<uint32_t>::max();
    uint32_t max = std::numeric_limits<uint32_t>::min();
    uint32_t avg = 0;
    uint64_t sum = 0;
    uint32_t count = 0;
    uint32_t window_end = 0;
    void add(window_counter_t *self, uint32_t value, uint16_t *telemetry, uint32_t seconds)
    {
        self->last = value;
        self->sum += value;
        add_deque_max(&self->max_deque, value);
        add_deque_min(&self->min_deque, value);

        if (self->count < self->duration)
        {
            self->count++;
        }
        else
        {
            auto last_index = std::max<uint32_t>(0, window_end - self->count);
            auto last = (uint32_t)telemetry[last_index];
            self->sum -= last;
            remove_last(&self->min_deque, last);
            remove_last(&self->max_deque, last);
        }

        self->max = max_deque.front();
        self->min = min_deque.front();

        self->window_end++;
        self->avg = (uint32_t)(self->sum / std::max<uint32_t>(1, self->count));
#ifdef ESP_LOGW
        ESP_LOGW("S", "c:%d, %d:%d, sum:%llu, avg:%d last:%d", self->count, seconds, window_end, self->sum, self->avg, window_end - self->count);
#endif
    }
    uint32_t get_min() { return min; }
    uint32_t get_max() { return max; }
    uint32_t get_avg() { return avg; }
    uint32_t get_count() { return count; }
    uint32_t get_duration() { return duration; }
};