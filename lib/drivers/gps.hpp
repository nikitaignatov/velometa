#ifndef _VELOHUB_GPS_HPP_
#define _VELOHUB_GPS_HPP_

#include <fmt/core.h>
#include <fmt/ranges.h>
#include "config.hpp"
#include "types.hpp"
#include "vh_display.hpp" // TODO: this should not be here
#include <tuple>
#include <float.h>
#include <math.h>
// #include <TinyGPSPlus.h>

#define NEOGPS_USE_SERIAL1
// #define GPS_FIX_TIME
// #define NMEAGPS_TIMESTAMP_FROM_INTERVAL
#include <NMEAGPS.h>
#include <GPSport.h>


void gps_task_code(void *parameter);
void gps_process_task_code(void *parameter);
#endif
