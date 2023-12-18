#include <fmt/core.h>
#include <fmt/ranges.h>
#include "unity.h"
#include <string>
#include <memory>
#include <array>
#include "window_counter.hpp"

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}

void test_min_value(void)
{
  std::array<uint16_t, 13> telemetry{{5, 1, 2, 3, 1, 8, 3, 9, 1, 2, 2, 3, 2}};
  // std::array<uint16_t, 100> telemetry={5, 1, 2, 3, 1, 8, 3, 9, 1, 2, 2, 3, 2,4,5,34,6,4,3,2,9,6,7,5,4,3,1,3,5};
  std::array<uint16_t, 13> min{{5, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 2, 2}};
  std::array<uint16_t, 13> max{{5, 5, 5, 3, 3, 8, 8, 9, 9, 9, 2, 3, 3}};
  std::array<uint16_t, 13> avg{{5, 3, 2, 2, 2, 4, 4, 6, 4, 4, 1, 2, 2}};
  window_counter_t w = {.duration = 3};

  for (size_t i = 0; i < telemetry.size(); i++)
  {
    w.add(&w, telemetry[i], (uint16_t *)telemetry.data(), i);
    UNITY_TEST_ASSERT_EQUAL_UINT16(min[i], w.min, 0, "failed window min");
    UNITY_TEST_ASSERT_EQUAL_UINT16(max[i], w.max, 0, "failed window max");
    UNITY_TEST_ASSERT_EQUAL_UINT16(avg[i], w.avg, 0, "failed window avg");
    printf("v: %d avg:%d min:%d max:%d\n", w.last, w.avg, w.min, w.max);
  }
}

int runUnityTests(void)
{

  UNITY_BEGIN();
  RUN_TEST(test_min_value);
  return UNITY_END();
}

int main(void)
{
  return runUnityTests();
}

void setup() {}
void loop() {}