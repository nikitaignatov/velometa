#include "unity.h"
#include <string>
#include "airspeed.hpp"


void tearDown(void){}
void setUp(void){}
void test_airspeed_density_0_calculation(void)
{
    Airspeed airspeed;
    UNITY_TEST_ASSERT_FLOAT_WITHIN(0.001, 0, airspeed.density(), 0, "invalid density");
}
void test_airspeed_density_calculation(void)
{
    Airspeed airspeed;
    airspeed.pressure(101325);
    airspeed.temperature(20);
    airspeed.humidity(60);
    
    UNITY_TEST_ASSERT_FLOAT_WITHIN(0.001, 1.219838, airspeed.density(), 0, "invalid density");
}

int runUnityTests(void)
{

    UNITY_BEGIN();
    RUN_TEST(test_airspeed_density_0_calculation);
    RUN_TEST(test_airspeed_density_calculation);

    return UNITY_END();
}

int main(void)
{
    return runUnityTests();
}

void setup() {}
void loop() {}