#pragma once

#include <deque>
#include <stdint.h>
#include <array>
#include <functional>
#include <math.h>

enum window_counter_type_t
{
    fixed_size_recent = 0,
    fixed_size_highest_avg,
};

struct window_counter_t
{
    std::deque<uint32_t> min_deque = {};
    std::deque<uint32_t> max_deque = {};
    uint32_t last;
    uint32_t min = std::numeric_limits<uint32_t>::max();
    uint32_t max = std::numeric_limits<uint32_t>::min();
    uint64_t sum = 0;
    uint64_t sum_w = 0;
    uint32_t count = 0;
    uint32_t window_end = 0;
    uint32_t duration = 0;

private:
    template <typename t_comparator>
    inline void add_deque(std::deque<uint32_t> *deque, uint32_t value, t_comparator compare);
    inline void add_deque_min(std::deque<uint32_t> *deque, uint32_t value);
    inline void add_deque_max(std::deque<uint32_t> *deque, uint32_t value);
    inline void remove_last(std::deque<uint32_t> *deque, uint32_t last);

public:
    void add(window_counter_t *self, uint32_t value, uint16_t *telemetry, uint32_t seconds);
    uint32_t get_min();
    uint32_t get_max();
    uint32_t get_avg();
    uint32_t get_avg_w();
    uint32_t get_last();
    uint32_t get_count();
    uint32_t get_position();
    uint32_t get_duration();
};