#ifndef _VELOHUB_TYPES_HPP_
#define _VELOHUB_TYPES_HPP_

#include "esp32-hal.h"
#include <stdint.h>
#include <array>
#include <vector>
#include <string>
#include <float.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "semphr.h"
#include "window_counter.hpp"

#define VH_SENSOR_BIT_MOCK_DATA 1
#define VH_SENSOR_BIT_GPS_READY 2
#define VH_SENSOR_BIT_HR_READY 4
#define VH_SENSOR_BIT_POWER_READY 8
#define VH_SENSOR_BIT_SPEED_READY 16
#define VH_SENSOR_BIT_CADENCE_READY 32

extern QueueHandle_t vh_raw_measurement_queue;
extern QueueHandle_t vh_metrics_queue;
extern QueueHandle_t vh_gps_queue;
extern EventGroupHandle_t sensor_status_bits;
extern SemaphoreHandle_t vh_display_semaphore;

typedef enum
{
    heartrate = 0,       // bpm
    power = 1,           // watt
    cadence = 2,         // rpm
    speed = 3,           // km/h
    distance = 4,        // km
    elevation = 5,       // m
    ascent = 6,          // m
    descent = 7,         // m
    slope = 8,           // %
    temperature = 9,     // °C
    pressure_abs = 10,   // mbar
    pressure_pitot = 11, // mbar
    humidity = 12,       // %
    vam = 13,            // %
    airspeed = 14,       // %
} measurement_t;

typedef struct
{
    measurement_t measurement;
    uint64_t ts;
    int32_t value;
    uint16_t scale;
} raw_measurement_msg_t;

typedef struct
{
    uint32_t ts;
    float value;
} metric_t;

typedef enum
{
    number = 0,
    chart = 0,
} datafield_type_t;

typedef struct
{
    datafield_type_t type;
    std::string label;

} datafield_t;

typedef struct
{
    std::vector<datafield_t> datafields;
} page_t;

typedef struct
{
    std::vector<page_t> pages;
} screen_t;

typedef struct
{
    std::string name;
    std::string address;
    int battery_pct;
    bool connected;
    bool enabled;
    uint64_t last_reading;
} sensor_state_t;

typedef struct
{
    sensor_state_t hr;
    sensor_state_t power;
    sensor_state_t speed;
    sensor_state_t gps;
    std::vector<std::string> near_by_devices;
} sensors_t;

typedef struct
{
    uint64_t tick_ms;
    uint64_t date;
    uint64_t time;
    double lat;
    double lon;
    double speed;
    double height;
    double hdop;
    double age;
    bool has_fix;
    uint32_t satelites;
    bool mocked;
} gps_data_t;

typedef struct
{
    uint32_t tick;
    double hr;
    double power;
    double speed;
    double cadence;
    double temperature;
    double pressure;
    double humidity;
    double lat;
    double lon;
    double speed_gps;
    double elevation_gps;
    uint32_t satelites;
    bool mocked;
} raw_telemetry_data_t;

typedef struct
{
    float *lat;
    float *lon;
    uint16_t *elevation;
    uint16_t *pressure;
    uint16_t *power;
    uint16_t *speed;
    uint16_t *hr;
    uint16_t *cadence;
    uint16_t *slope;
} ride_data_t;


extern void publish_gps(gps_data_t data);

#endif