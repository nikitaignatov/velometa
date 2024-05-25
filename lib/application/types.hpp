#ifndef _VELOHUB_TYPES_HPP_
#define _VELOHUB_TYPES_HPP_

#include "esp32-hal.h"
#include <stdint.h>
#include <array>
#include <vector>
#include <optional>
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

class System;
extern System sys;
extern uint64_t ts();

extern QueueHandle_t vh_raw_measurement_queue;
extern QueueHandle_t vh_metrics_queue;
extern QueueHandle_t vh_gps_csv_queue;
extern QueueHandle_t vh_gps_queue;
extern QueueHandle_t vm_csv_queue;

extern EventGroupHandle_t sensor_status_bits;
extern SemaphoreHandle_t vh_display_semaphore;
extern SemaphoreHandle_t vm_sdcard_semaphor;

typedef enum
{
    heartrate = 0,      // bpm
    power = 1,          // watt
    cadence = 2,        // rpm
    speed = 3,          // km/h
    distance = 4,       // km
    elevation = 5,      // m
    ascent = 6,         // m
    descent = 7,        // m
    slope = 8,          // %
    vam,                // m
    air_temperature,    // °C
    air_pressure_abs,   // Pa
    air_humidity,       // %
    air_speed,          // km/h
    air_density,        // kg/m3
    diff_pressure_l_pa, // Pa
    diff_pressure_r_pa, // Pa
    ax_ms2,             // m/s2
    ay_ms2,             // m/s2
    az_ms2,             // m/s2
    wind_speed,         // km/h
    wind_yaw,           // deg
    gx_radps,           // radps
    gy_radps,           // radps
    gz_radps,           // radps
    mx_ut,              // ut
    my_ut,              // ut
    mz_ut,              // ut
    roll,               //
    pitch,              //
    yaw,                //
    position_mm,
    duration_ms,
    qw,
    qx,
    qy,
    qz,
    arx,
    ary,
    arz,
    awx,
    awy,
    awz,
    gravityx,
    gravityy,
    gravityz,
} measurement_t;

typedef struct
{
    uint64_t ts;
    measurement_t measurement;
    float value;
} raw_measurement_msg_t;

// const char* p = "1715879083889,16,-15.093266";

// constexpr size_t sizeOfT = sizeof("1715879083889,16,-15.093266");

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
    float lat;
    float lon;
    float speed;
    int32_t height;
    float heading;
    float distance;
    uint16_t hdop;
    uint16_t vdop;
    uint32_t age;
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

void publish_measurement(std::optional<float> input, measurement_t type, uint64_t ts, bool screen = true);

#endif