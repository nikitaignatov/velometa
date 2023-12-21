#include "window_counter.hpp"
#include <iostream>

inline void window_counter_t::add_deque_min(std::deque<uint32_t> *deque, uint32_t value) { add_deque(deque, value, std::less{}); }
inline void window_counter_t::add_deque_max(std::deque<uint32_t> *deque, uint32_t value) { add_deque(deque, value, std::greater{}); }

template <typename t_comparator>
inline void window_counter_t::add_deque(std::deque<uint32_t> *deque, uint32_t value, t_comparator compare)
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

inline void window_counter_t::remove_last(std::deque<uint32_t> *deque, uint32_t last)
{
    if (deque->size() > 1 && deque->front() == last)
    {
        deque->pop_front();
    }
}

void window_counter_t::add(window_counter_t *self, uint32_t value, uint16_t *telemetry, uint32_t seconds)
{
    if (self->count == self->duration)
    {
        auto last_index = std::max<uint32_t>(0, window_end - self->count);
        auto last_value = (uint32_t)telemetry[last_index];
        self->sum -= last_value;
        self->sum_w -= std::pow(last_value, 4);
        remove_last(&self->min_deque, last_value);
        remove_last(&self->max_deque, last_value);
    }
    else
    {
        self->count++;
    }

    self->last = value;
    self->sum += value;
    self->sum_w += std::pow(value, 4);

    add_deque_max(&self->max_deque, value);
    add_deque_min(&self->min_deque, value);

    self->max = max_deque.front();
    self->min = min_deque.front();
    self->window_end++;
}

uint32_t window_counter_t::get_avg()
{
    return count == 0 ? 0 : static_cast<uint32_t>(sum / count);
}
uint32_t window_counter_t::get_avg_w()
{
    // return get_avg();
    return static_cast<uint32_t>(std::pow(static_cast<double>(sum_w) / count, 0.25));
}

uint32_t window_counter_t::get_last() { return last; }
uint32_t window_counter_t::get_count() { return count; }
uint32_t window_counter_t::get_position() { return window_end; }
uint32_t window_counter_t::get_duration() { return duration; }
uint32_t window_counter_t::get_min() { return min_deque.front(); }
uint32_t window_counter_t::get_max() { return max_deque.front(); }