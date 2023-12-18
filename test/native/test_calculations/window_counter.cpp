#include <fmt/core.h>
#include <fmt/ranges.h>
#include "unity.h"
#include <string>
#include <memory>
#include <array>
#include "window_counter.hpp"

static std::array<uint16_t, 13> input_telemetry_13{{5, 1, 2, 3, 1, 8, 3, 9, 1, 2, 2, 3, 2}};

void test_window_3_min_max_avg_last(void)
{
    window_counter_t w{.duration = 3};
    static std::array<uint16_t, 13> expect_min{{5, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 2, 2}};
    static std::array<uint16_t, 13> expect_max{{5, 5, 5, 3, 3, 8, 8, 9, 9, 9, 2, 3, 3}};
    static std::array<uint32_t, 13> expect_avg{{5, 3, 2, 2, 2, 4, 4, 6, 4, 4, 1, 2, 2}};

    for (size_t i = 0; i < input_telemetry_13.size(); i++)
    {
        w.add(&w, input_telemetry_13[i], (uint16_t *)input_telemetry_13.data(), i);
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_min[i], w.get_min(), 0, "failed window min");
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_max[i], w.get_max(), 0, "failed window max");
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_avg[i], w.get_avg(), 0, "failed window avg");
        UNITY_TEST_ASSERT_EQUAL_UINT16(input_telemetry_13[i], w.get_last(), 0, "failed window last");
        printf("v: %d avg:%d avgw:%d min:%d max:%d\n", w.get_last(), w.get_avg(), w.get_avg_w(), w.get_min(), w.get_max());
    }
}

void test_window_5_min_max_avg_last(void)
{
    window_counter_t w{.duration = 5};
    static std::array<uint16_t, 13> expect_min{{5, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1}};
    static std::array<uint16_t, 13> expect_max{{5, 5, 5, 5, 5, 8, 8, 9, 9, 9, 9, 9, 3}};
    static std::array<uint32_t, 13> expect_avg{{5, 3, 2, 2, 2, 3, 3, 4, 4, 4, 3, 3, 2}};

    for (size_t i = 0; i < input_telemetry_13.size(); i++)
    {
        w.add(&w, input_telemetry_13[i], (uint16_t *)input_telemetry_13.data(), i);
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_min[i], w.get_min(), 0, "failed window min");
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_max[i], w.get_max(), 0, "failed window max");
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_avg[i], w.get_avg(), 0, "failed window avg");
        UNITY_TEST_ASSERT_EQUAL_UINT16(input_telemetry_13[i], w.get_last(), 0, "failed window last");
        printf("v: %d avg:%d avgw:%d min:%d max:%d\n", w.get_last(), w.get_avg(), w.get_avg_w(), w.get_min(), w.get_max());
    }
}

void test_window_3600_min_max_avg_last(void)
{
    window_counter_t w{.duration = 3600};
    static std::array<uint16_t, 13> expect_min{{5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    static std::array<uint16_t, 13> expect_max{{5, 5, 5, 5, 5, 8, 8, 9, 9, 9, 9, 9, 9}};
    static std::array<uint32_t, 13> expect_avg{{5, 3, 2, 2, 2, 3, 3, 4, 3, 3, 3, 3, 3}};

    for (size_t i = 0; i < input_telemetry_13.size(); i++)
    {
        w.add(&w, input_telemetry_13[i], (uint16_t *)input_telemetry_13.data(), i);
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_min[i], w.get_min(), 0, "failed window min");
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_max[i], w.get_max(), 0, "failed window max");
        UNITY_TEST_ASSERT_EQUAL_UINT16(expect_avg[i], w.get_avg(), 0, "failed window avg");
        UNITY_TEST_ASSERT_EQUAL_UINT16(input_telemetry_13[i], w.get_last(), 0, "failed window last");
        printf("v: %d avg:%d avgw:%d min:%d max:%d\n", w.get_last(), w.get_avg(), w.get_avg_w(), w.get_min(), w.get_max());
    }
}

int runUnityTests(void)
{

    UNITY_BEGIN();
    RUN_TEST(test_window_3_min_max_avg_last);
    RUN_TEST(test_window_5_min_max_avg_last);
    RUN_TEST(test_window_3600_min_max_avg_last);

    return UNITY_END();
}

int main(void)
{
    return runUnityTests();
}

void setup() {}
void loop() {}