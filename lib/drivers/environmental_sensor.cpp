#include "environmental_sensor.hpp"

static const char *TAG = "environmental_sensor";

std::optional<float> EnvironmentalSensor::get_air_pressure()
{
    auto invalid = std::optional<float>();
    auto max_pressure = 110000.0f;
    auto min_pressure = 30000.0f;
    auto p = sensor.readPressure();

    if (p > max_pressure)
    {
        return invalid;
    }
    if (p < min_pressure)
    {
        return invalid;
    }
    return p;
}

std::optional<float> EnvironmentalSensor::get_air_temperature()
{
    auto invalid = std::optional<float>();
    auto max_temp = 85.0f;
    auto min_temp = -40.0f;
    auto t = sensor.readTemperature();

    if (t > max_temp)
    {
        return invalid;
    }
    if (t < min_temp)
    {
        return invalid;
    }
    return t;
}

std::optional<float> EnvironmentalSensor::get_air_relative_humidity()
{
    auto invalid = std::optional<float>();
    auto max_humidity = 100.0f;
    auto min_humidity = 0.0f;
    auto rh = sensor.readHumidity();

    if (rh > max_humidity)
    {
        return invalid;
    }
    if (rh < min_humidity)
    {
        return invalid;
    }
    return rh;
}

void EnvironmentalSensor::init(uint8_t address)
{
    Wire.end();
    Wire.begin(ENVIRONMENTAL_SENSOR_SDA, ENVIRONMENTAL_SENSOR_SCL, 400000);
    auto status = sensor.begin(address, &Wire);

    if (!status)
    {
        ESP_LOGE(TAG, "Could not find a valid BME280 sensor, check wiring!");
    }

    // recommended settings for indoor navigation
    // lowest possible altitude noise is needed.
    sensor.setSampling(
        Adafruit_BME280::MODE_NORMAL,  // mode
        Adafruit_BME280::SAMPLING_X2,  // temperature
        Adafruit_BME280::SAMPLING_X16, // pressure
        Adafruit_BME280::SAMPLING_X4,  // humidity
        Adafruit_BME280::FILTER_X16,
        Adafruit_BME280::STANDBY_MS_0_5);
}