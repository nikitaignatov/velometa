#include "unity.h"
#include "zones.h"
#include <string>
#include <format>

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}

void test_power_zone_0w(void)
{
  uint16_t watt = 0;
  uint8_t actual = calculate_power_zone(watt, 100);
  uint8_t expected = 1;
  UNITY_TEST_ASSERT_INT8_WITHIN(1, expected, actual, 0, "Zone 1 for 0 power.");
}

void test_power_zone_range(uint8_t expected, uint16_t threshold_value_min, uint16_t threshold_value_max)
{
  uint16_t max = 2000;
  for (size_t watt = threshold_value_min; watt < threshold_value_max; watt++)
  {
    for (size_t ftp_multiplier = 1; ftp_multiplier < 5; ftp_multiplier++)
    {
      // arrange
      auto msg = std::format("Zone {} for {} power. {}*{}", expected, watt * ftp_multiplier, watt, ftp_multiplier);

      // act
      uint8_t actual = calculate_power_zone(watt*ftp_multiplier, 100 * ftp_multiplier);

      // asset
      UNITY_TEST_ASSERT_INT8_WITHIN(1, expected, actual, 0, msg.data());
    }
  }
}

void test_power_zones_range(void)
{
  test_power_zone_range(1, POWER_Z1_MIN, POWER_Z2_MIN);
  test_power_zone_range(2, POWER_Z2_MIN, POWER_Z3_MIN);
  test_power_zone_range(3, POWER_Z3_MIN, POWER_Z4_MIN);
  test_power_zone_range(4, POWER_Z4_MIN, POWER_Z5_MIN);
  test_power_zone_range(5, POWER_Z5_MIN, POWER_Z6_MIN);
  test_power_zone_range(6, POWER_Z6_MIN, POWER_Z7_MIN);
  test_power_zone_range(6, POWER_Z7_MIN, 2000);
}

int runUnityTests(void)
{

  UNITY_BEGIN();
  RUN_TEST(test_power_zone_0w);
  RUN_TEST(test_power_zones_range);
  return UNITY_END();
}

int main(void)
{
  return runUnityTests();
}
