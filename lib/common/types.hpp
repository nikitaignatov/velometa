#ifndef _VELOHUB_TYPES_HPP_
#define _VELOHUB_TYPES_HPP_

#include <stdint.h>
#include <vector>
#include <string>
#include <float.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

extern QueueHandle_t vh_raw_measurement_queue;
extern QueueHandle_t vh_metrics_queue;

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

#endif