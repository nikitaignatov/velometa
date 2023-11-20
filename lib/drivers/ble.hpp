#ifndef _VELOHUB_BLE_HPP_
#define _VELOHUB_BLE_HPP_

#include <vector>
#include "config.hpp"
#include "types.hpp"
#include <Arduino.h>
#include <BLEDevice.h>

static auto missing_address = BLEAddress((uint8_t *)"\0\0\0\0\0\0");

enum metric_type_t
{
    METRIC_NOT_DEFINED = 0,
    HR_BPM,
    POWER_WATT,
    POWER_CRANK_RPM,
    SPEED_WHEEL_RPM,
    SPEED_CRANK_RPM,
};

typedef struct
{
    char device_name[20];
    metric_type_t metric;
    BLEUUID service_id;
    BLEUUID characteristic_id;
    BLEAddress address;
    BLEClient *client;
    void (*parse_data)(uint8_t *pData, size_t length);
    bool enabled;
} sensor_definition_t;

extern std::vector<sensor_definition_t> ble_sensors;

void ble_task_code(void *parameter);

void ble_parse_hr_data(uint8_t *pData, size_t length);
void ble_parse_power_watt_data(uint8_t *pData, size_t length);
void ble_parse_speed_wheel_rpm_data(uint8_t *pData, size_t length);

#endif