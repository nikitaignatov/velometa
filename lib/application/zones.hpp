#ifndef _VELOHUB_ZONES_HPP_
#define _VELOHUB_ZONES_HPP_

#include <float.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t calculate_power_zone(uint16_t power, uint16_t ftp);
uint8_t calculate_hr_zone(uint16_t hr);

// HR

#define HR_Z1_MIN 0
#define HR_Z1_MAX 118

#define HR_Z2_MIN 119
#define HR_Z2_MAX 156

#define HR_Z3_MIN 157
#define HR_Z3_MAX 175

#define HR_Z4_MIN 176
#define HR_Z4_MAX 194

#define HR_Z5_MIN 195
#define HR_Z5_MAX 250

// Power

#define FTP 270

#define POWER_Z1_MIN 0

#define POWER_Z2_MIN 55

#define POWER_Z3_MIN 76

#define POWER_Z4_MIN 91

#define POWER_Z5_MIN 106

#define POWER_Z6_MIN 121

#define POWER_Z7_MIN 151

#define POWER_ZSS_MIN 84
#define POWER_ZSS_MAX 97


#endif