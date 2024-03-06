#ifndef _VELOHUB_BLE_HPP_
#define _VELOHUB_BLE_HPP_

#define BTA_GATTC_CONN_MAX 5
#define BTA_GATTC_NOTIF_REG_MAX 15

#include <fmt/core.h>
#include <fmt/ranges.h>
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
    AIRSPEED_KMH,
    AIRSPEED_MS,
    AIR_PRESSURE_PA,
    AIR_TEMP_C,
    AIR_DENSITY_KG_M3,
    AIR_RH_PCT,
    ALTITUDE_M,
    DIFF_PRESSURE_L_PA,
    DIFF_PRESSURE_R_PA,
    AX_MS2,
    AY_MS2,
    AZ_MS2,
};

typedef struct
{
    std::string device_name;
    metric_type_t metric;
    BLEUUID service_id;
    std::map<measurement_t, BLEUUID> characteristic_id;
    BLEAddress address;
    BLEClient *client;
    void (*parse_data)(uint8_t *pData, size_t length, BLERemoteCharacteristic *characteristic);
    bool enabled;
    sensor_state_t state;
    esp_ble_addr_type_t address_type;
    bool has_notification = true;

} sensor_definition_t;

extern std::vector<sensor_definition_t> ble_sensors;

void ble_task_code(void *parameter);

void ble_parse_hr_data(uint8_t *pData, size_t length, BLERemoteCharacteristic *characteristic);
void ble_parse_power_watt_data(uint8_t *pData, size_t length, BLERemoteCharacteristic *characteristic);
void ble_parse_speed_wheel_rpm_data(uint8_t *pData, size_t length, BLERemoteCharacteristic *characteristic);
void ble_parse_airspeed(uint8_t *pData, size_t length, BLERemoteCharacteristic *characteristic);

extern std::map<measurement_t, BLEUUID> airspeed_metrics_def;
#endif