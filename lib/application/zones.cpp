#include "zones.hpp"

uint8_t calculate_power_zone(uint16_t power, uint16_t ftp)
{
    uint16_t pct = (power / (float)ftp) * 100.0;
    if (pct < POWER_Z2_MIN) return 1;
    else if (pct >= POWER_Z2_MIN && pct < POWER_Z3_MIN) return 2;
    else if (pct >= POWER_Z3_MIN && pct < POWER_Z4_MIN) return 3;
    else if (pct >= POWER_Z4_MIN && pct < POWER_Z5_MIN) return 4;
    else if (pct >= POWER_Z5_MIN && pct < POWER_Z6_MIN) return 5;
    else if (pct >= POWER_Z6_MIN && pct < POWER_Z7_MIN) return 6;
    else if (pct >= POWER_Z7_MIN) return 7;
    return 0;
}

uint8_t calculate_hr_zone(uint16_t hr)
{
    if (HR_Z1_MIN <= hr && HR_Z1_MAX >= hr) return  1;
    if (HR_Z2_MIN <= hr && HR_Z2_MAX >= hr) return  2;
    if (HR_Z3_MIN <= hr && HR_Z3_MAX >= hr) return  3;
    if (HR_Z4_MIN <= hr && HR_Z4_MAX >= hr) return  4;
    if (HR_Z5_MIN <= hr ) return  5;
    return 1;
}
