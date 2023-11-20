#ifndef VELOHUB_ZONES_H
#define VELOHUB_ZONES_H

#include <float.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
typedef float float_t;

uint8_t calculate_power_zone(uint16_t power, uint16_t ftp);
uint8_t calculate_hr_zone(uint16_t hr);

// HR

#define HR_Z1_MIN 0
#define HR_Z1_MAX 116

#define HR_Z2_MIN 117
#define HR_Z2_MAX 155

#define HR_Z3_MIN 156
#define HR_Z3_MAX 173

#define HR_Z4_MIN 174
#define HR_Z4_MAX 193

#define HR_Z5_MIN 193
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