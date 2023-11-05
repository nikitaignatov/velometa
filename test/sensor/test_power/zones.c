#include "unity.h"
#include "zones.h"

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
  uint8_t actual = calculate_power_zone(watt);
  uint8_t expected = 1;
  UNITY_TEST_ASSERT_INT8_WITHIN(1, expected, actual, 0, "Zone 1 for 0 power.");
}

void test_power_zone_1_range(void)
{
  uint8_t actual = calculate_power_zone(0);
  UNITY_TEST_ASSERT_INT8_WITHIN(1, 1, actual, 0, "Zone 1 for 0 power.");
}

int runUnityTests(void)
{

  UNITY_BEGIN();
  RUN_TEST(test_power_zone_0w);
  RUN_TEST(test_power_zone_1_range);
  return UNITY_END();
}

// WARNING!!! PLEASE REMOVE UNNECESSARY MAIN IMPLEMENTATIONS //

/**
  * For native dev-platform or for some embedded frameworks
  */
int main(void)
{
  return runUnityTests();
}

/**
  * For Arduino framework
  */
void setup()
{
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface

  runUnityTests();
}
void loop() {}

/**
  * For ESP-IDF framework
  */
void app_main()
{
  runUnityTests();
}