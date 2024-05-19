#include "differential_pressure_sensor.hpp"
static const char *TAG = "differential_pressure_sensor";

std::optional<float> DifferentialPressureSensor::get_pressure()
{
    return _pressure;
}

std::optional<float> DifferentialPressureSensor::get_temperature()
{
    return _temperature;
}

/// @brief Read the sensor data from I2C bus and validate the values are within specified range
/// @return true if all values are within operatinal range otherwise false.
bool DifferentialPressureSensor::read_sensor()
{
    // https://cfsensor.com/wp-content/uploads/2022/11/XGZP6897D-Pressure-Sensor-V2.7.pdf
    auto max_pressure = 1000;
    auto min_pressure = -1000;
    auto max_temperature = 100;
    auto min_temperature = -20;
    change_port(address);
    sensor.readSensor(_temperature_raw, _pressure_raw);
    _pressure = std::optional<float>();
    _temperature = std::optional<float>();

    if (_temperature_raw > max_temperature)
    {
        ESP_LOGE(TAG, "Received temp[%.2f] above max_operational[%.2f].", _temperature_raw, max_temperature);
        return false;
    }
    if (_temperature_raw < min_temperature)
    {
        ESP_LOGE(TAG, "Received temp[%.2f] below min_operational[%.2f].", _temperature_raw, min_temperature);
        return false;
    }
    _temperature = _temperature_raw;

    if (_pressure_raw > max_pressure)
    {
        ESP_LOGE(TAG, "Received pressure[%.2f] above max_operational[%.2f].", _pressure_raw, max_pressure);
        return false;
    }
    if (_pressure_raw < min_pressure)
    {
        ESP_LOGE(TAG, "Received pressure[%.2f] below min_operational[%.2f].", _pressure_raw, min_pressure);
        return false;
    }
    _pressure = _pressure_raw - _offset;

    return true;
}

void DifferentialPressureSensor::change_port(uint8_t port)
{
    if (port == 0)
    {
        Wire.end();
        Wire.begin(VM_I2C_0_SDA, VM_I2C_0_SCL, 400000);
    }
    else if (port == 1)
    {
        Wire.end();
        Wire.begin(VM_I2C_1_SDA, VM_I2C_1_SCL, 400000);
    }
}

void DifferentialPressureSensor::init(uint8_t _address)
{
    address = _address;

    change_port(_address);

    if (!sensor.begin()) // initialize and check the device
    {
        ESP_LOGE(TAG, "Could not find a valid XGZP6897D sensor, check wiring!");
    }
    int count = 0;
    while (count < 10)
    {
        count++;

        sensor.readSensor(_temperature_raw, _pressure_raw);
        if (count == 1)
        {
            _offset = _pressure_raw;
        }
        else
        {
            _offset = (_offset + _pressure_raw) / 2;
        }
    }
    ESP_LOGE(TAG, "XGZP6897D[%d] with offset[%f] initialized", address, _offset);
}