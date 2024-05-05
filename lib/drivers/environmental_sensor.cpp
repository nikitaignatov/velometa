#include "environmental_sensor.hpp"

static const char *TAG = "environmental_sensor";

bool EnvironmentalSensor::read_sensor()
{
    // init(_address);
    return true;
}

std::optional<float> EnvironmentalSensor::get_air_pressure()
{
    auto invalid = std::optional<float>();
    auto max_pressure = 110000.0f;
    auto min_pressure = 30000.0f;
    auto p = sensor.readPressure();
    _pressure = invalid;
    if (p > max_pressure)
    {
        return invalid;
    }
    if (p < min_pressure)
    {
        return invalid;
    }
    _pressure = p;
    return p;
}

std::optional<float> EnvironmentalSensor::get_air_temperature()
{
    auto invalid = std::optional<float>();
    auto max_temp = 85.0f;
    auto min_temp = -40.0f;
    auto t = sensor.readTemperature();

    _temperature = invalid;
    if (t > max_temp)
    {
        return invalid;
    }
    if (t < min_temp)
    {
        return invalid;
    }
    _temperature = t;
    return t;
}

std::optional<float> EnvironmentalSensor::get_air_relative_humidity()
{
    auto invalid = std::optional<float>();
    auto max_humidity = 100.0f;
    auto min_humidity = 0.0f;
    auto rh = sensor.readHumidity();
    _humidity = invalid;

    if (rh > max_humidity)
    {
        return invalid;
    }
    if (rh < min_humidity)
    {
        return invalid;
    }
    _humidity = rh;
    return rh;
}

static float calculate_air_density(float T, float h, float Pa)
{
    float k = 273.13;
    float Rd = 287.058; // gas constant for dry air, J/(kg*degK)
    float Rv = 461.495; //	gas constant for water vapor, J/(kg*degK)

    float Tk = T + k;
    float a = 7.5 * T;
    float b = T + 237.3;
    float Pvs = 6.1078 * 10 * (a / b);
    float Pva = Pvs * h; // pressure of water vapor (partial pressure), Pascals
    float Pd = Pa - Pva; // pressure of dry air (partial pressure), Pascals
    float x = Pd / Rd * Tk;
    float y = Pva / Rv * Tk;
    float p = x + y;      // total air pressure, Pascals ( multiply mb by 100 to get Pascals)
    return p / (Rd * Tk); // kg/m3
}

std::optional<float> EnvironmentalSensor::get_air_density()
{
    if (_pressure.has_value() && _temperature.has_value() && _humidity.has_value())
    {
        return calculate_air_density(_temperature.value(), _humidity.value(), _pressure.value());
    }
    else
    {
        return std::optional<float>();
    }
}

void EnvironmentalSensor::init(uint8_t address)
{
    _address = address;
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