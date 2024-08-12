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


#define NEOGPS_USE_SERIAL1
#define GPS_FIX_TIME
#define NMEAGPS_TIMESTAMP_FROM_INTERVAL

#define NMEAGPS_PARSE_GGA
#define NMEAGPS_PARSE_GLL
#define NMEAGPS_PARSE_GSA
#define NMEAGPS_PARSE_GSV
#define NMEAGPS_PARSE_GST
#define NMEAGPS_PARSE_RMC
#define NMEAGPS_PARSE_VTG
#define NMEAGPS_PARSE_ZDA

#define GPS_FIX_DATE
#define GPS_FIX_TIME
#define GPS_FIX_LOCATION
//#define GPS_FIX_LOCATION_DMS
#define GPS_FIX_ALTITUDE
#define GPS_FIX_SPEED
//#define GPS_FIX_VELNED
#define GPS_FIX_HEADING
#define GPS_FIX_SATELLITES
#define GPS_FIX_HDOP
#define GPS_FIX_VDOP
#define GPS_FIX_PDOP
//#define GPS_FIX_LAT_ERR
//#define GPS_FIX_LON_ERR
//#define GPS_FIX_ALT_ERR
//#define GPS_FIX_SPD_ERR
//#define GPS_FIX_HDG_ERR
//#define GPS_FIX_TIME_ERR
#define GPS_FIX_GEOID_HEIGHT

#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_RMC


#include <NMEAGPS.h>
#include <GPSport.h>

uint64_t ts();
void gps_task_code(void *parameter);
void gps_process_task_code(void *parameter);
#endif
