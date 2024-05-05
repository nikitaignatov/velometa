#include "sensor_reader.hpp"
static const char *TAG = "sensor_reader";

static EnvironmentalSensor environmental;
static PositionTrackingSensor position;
static DifferentialPressureSensor diff_pressure_1;
static DifferentialPressureSensor diff_pressure_2;

void sensor_reader_task_code(void *parameter)
{
    esp_log_level_set("ARDUINO", ESP_LOG_WARN); // due to reusing same port for different pins
    ESP_LOGI(TAG, "sensor_reader_task_code");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    environmental.init(ENVIRONMENTAL_SENSOR_ADDRESS);
    auto Pa = environmental.get_air_pressure().value_or(-1);
    auto T = environmental.get_air_temperature().value_or(-1);
    auto h = environmental.get_air_relative_humidity().value_or(-1);
    ESP_LOGI(TAG, "Env sesnor reading: %.2f C; %.1f RH; %f Pa;\n", T, h, Pa);
    diff_pressure_1.init(1);
    diff_pressure_2.init(0);
    ESP_LOGI(TAG, "Diff sesnor initialized.\n");
    // position.init(POSITION_SENSOR_ADDRESS);
    ESP_LOGI(TAG, "Position sesnor initialized.\n");

    for (;;)
    {
        auto measurement_ts = ts();

        if (diff_pressure_1.read_sensor())
        {
            publish_measurement(diff_pressure_1.get_pressure(), measurement_t::diff_pressure_l_pa, measurement_ts);
        }

        if (diff_pressure_2.read_sensor())
        {
            publish_measurement(diff_pressure_2.get_pressure(), measurement_t::diff_pressure_r_pa, measurement_ts);
        }

        publish_measurement(environmental.get_air_pressure(), measurement_t::air_pressure_abs, measurement_ts);
        publish_measurement(environmental.get_air_temperature(), measurement_t::air_temperature, measurement_ts);
        publish_measurement(environmental.get_air_relative_humidity(), measurement_t::air_humidity, measurement_ts);

        publish_measurement(position.get_distance_mm(), measurement_t::position_mm, measurement_ts);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}