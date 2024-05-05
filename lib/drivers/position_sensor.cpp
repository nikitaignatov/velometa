#include "position_sensor.hpp"

static const char *TAG = "position_sensor";

bool PositionTrackingSensor::read_sensor()
{
    _distance = std::optional<float>();
    if (ready)
    {
        VL53L0X_RangingMeasurementData_t measure;
        sensor.rangingTest(&measure, debug);

        if (measure.RangeStatus != 4)
        {
            auto x = measure.RangeMilliMeter;
            y = (a * y) + (x - (a * x));
            if (debug)
            {
                ESP_LOGV(TAG, "VL53L0X: %.2f", y);
            }
            _distance = y;
            return true;
        }
        else
        {
            ESP_LOGE(TAG, "VL53L0X: Out of range");
            return false;
        }
    }

    return false;
}

std::optional<float> PositionTrackingSensor::get_distance_mm()
{
    return _distance;
}

void PositionTrackingSensor::init(uint8_t address)
{
    ESP_LOGI(TAG, "VL53L0X test");
    Wire.end();
    Wire.begin(POSITION_SENSOR_SDA, POSITION_SENSOR_SCL, 400000);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    if (!sensor.begin(address))
    {
        ESP_LOGE(TAG, "Failed to boot VL53L0X r");
        ready = false;
    }
    else
    {
        ready = true;
    }
}
