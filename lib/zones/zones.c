#include "zones.h"

uint8_t calculate_power_zone(uint16_t power)
{
    float_t ftp = (float_t)FTP;
    uint16_t pct = power / ftp * 100.0;
    uint8_t zone = 0;
    if (POWER_Z1_MIN < pct && POWER_Z1_MAX > pct) zone = 1;
    else if (POWER_Z2_MIN < pct && POWER_Z2_MAX > pct) zone = 2;
    else if (POWER_Z3_MIN < pct && POWER_Z3_MAX > pct) zone = 3;
    else if (POWER_Z4_MIN < pct && POWER_Z4_MAX > pct) zone = 4;
    else if (POWER_Z5_MIN < pct && POWER_Z5_MAX > pct) zone = 5;
    else if (POWER_Z6_MIN < pct && POWER_Z6_MAX > pct) zone = 6;
    else if (POWER_Z7_MIN < pct) zone = 7;
    return zone;
}
