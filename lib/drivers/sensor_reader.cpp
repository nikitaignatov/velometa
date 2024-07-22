#include "sensor_reader.hpp"
static const char *TAG = "sensor_reader";

static EnvironmentalSensor environmental;
static PositionTrackingSensor position;
static DifferentialPressureSensor diff_pressure_1;
static DifferentialPressureSensor diff_pressure_2;
static IMU imu;

static std::optional<float> to_ms(std::optional<float> _dp, std::optional<float> _density)
{
    if (_dp.has_value() && _density.has_value())
    {
        float dp = _dp.value();
        float density = _density.value();
        float h = 0.010;
        float inlet = h * (30.0 / 100.0);
        float throat = h * (13.5 / 100.0);
        float ratio = inlet / throat;
        float gain = 5.0;
        // float inlet = PI * pow(24.0 / 2, 2);
        // float throat = PI * pow(18.0 / 2, 2);
        // float ratio = inlet / throat;

        if (dp == 0)
            return 0;
        else if (dp < 0)
            return sqrt(((2 * -(dp * gain)) / ((pow(ratio, 2) - 1) * density)));
        return -sqrt(((2 * ((dp * gain)) / ((pow(ratio, 2) - 1) * density))));
    }
    return std::optional<float>();
}

void sensor_reader_task_code(void *parameter)
{
    ESP_LOGW(TAG, "sensor_reader_task_code");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    environmental.init(ENVIRONMENTAL_SENSOR_ADDRESS);
    auto Pa = environmental.get_air_pressure().value_or(-1);
    auto T = environmental.get_air_temperature().value_or(-1);
    auto h = environmental.get_air_relative_humidity().value_or(-1);
    ESP_LOGW(TAG, "Env sesnor reading: %.2f C; %.1f RH; %f Pa;\n", T, h, Pa);

    diff_pressure_2.init(0);
    diff_pressure_1.init(1);

    ESP_LOGW(TAG, "Diff sesnor initialized.\n");
    position.init(POSITION_SENSOR_ADDRESS);
    ESP_LOGW(TAG, "Position sesnor initialized.\n");
    // imu.init();
    ESP_LOGW(TAG, "IMU sesnor initialized.\n");

    std::optional<float> p;
    std::optional<float> d;
    std::optional<float> ms;
    uint64_t duration_ms = 0;
    for (;;)
    {
        auto measurement_ts = ts();

        if (diff_pressure_1.read_sensor())
        {
            publish_measurement(diff_pressure_1.get_pressure(), measurement_t::diff_pressure_l_pa, measurement_ts);
            p = diff_pressure_1.get_pressure();
        }

        if (diff_pressure_2.read_sensor())
        {
            publish_measurement(diff_pressure_2.get_pressure(), measurement_t::diff_pressure_r_pa, measurement_ts);
            auto x = diff_pressure_2.get_pressure();
            if (x.has_value() && p.has_value())
            {
                auto yaw = atan2(x.value_or(0.1), p.value_or(0.1)) * RAD_TO_DEG;
                publish_measurement(yaw, measurement_t::yaw, measurement_ts);
            }
        }

        if (environmental.read_sensor())
        {
            publish_measurement(environmental.get_air_pressure(), measurement_t::air_pressure_abs, measurement_ts);
            publish_measurement(environmental.get_air_temperature(), measurement_t::air_temperature, measurement_ts);
            publish_measurement(environmental.get_air_relative_humidity(), measurement_t::air_humidity, measurement_ts);
            publish_measurement(environmental.get_elevation(), measurement_t::elevation, measurement_ts);
            publish_measurement(environmental.get_air_density(), measurement_t::air_density, measurement_ts);
            d = environmental.get_air_density();

            ms = to_ms(p, d);
            if (ms.has_value())
            {
                std::optional<float> kmh = ms.value() * 3.6;
                publish_measurement(kmh, measurement_t::air_speed, measurement_ts);
            }
        }

        if (position.read_sensor())
        {
            publish_measurement(position.get_distance_mm(), measurement_t::position_mm, measurement_ts);
        }

        if (imu.read_sensor())
        {
            publish_measurement(imu.get_roll(), measurement_t::roll, measurement_ts);
            publish_measurement(imu.get_pitch(), measurement_t::pitch, measurement_ts);
            // publish_measurement(imu.get_yaw(), measurement_t::yaw, measurement_ts, false);

            // publish_measurement(imu.aa.x, measurement_t::ax_ms2, measurement_ts, false);
            // publish_measurement(imu.aa.y, measurement_t::ay_ms2, measurement_ts, false);
            // publish_measurement(imu.aa.z, measurement_t::az_ms2, measurement_ts, false);

            // publish_measurement(imu.gg.x, measurement_t::gx_radps, measurement_ts, false);
            // publish_measurement(imu.gg.y, measurement_t::gy_radps, measurement_ts, false);
            // publish_measurement(imu.gg.z, measurement_t::gz_radps, measurement_ts, false);

            // publish_measurement(imu.aaReal.x, measurement_t::arx, measurement_ts, false);
            // publish_measurement(imu.aaReal.y, measurement_t::ary, measurement_ts, false);
            // publish_measurement(imu.aaReal.z, measurement_t::arz, measurement_ts, false);

            // publish_measurement(imu.aaWorld.x, measurement_t::awx, measurement_ts, false);
            // publish_measurement(imu.aaWorld.y, measurement_t::awy, measurement_ts, false);
            // publish_measurement(imu.aaWorld.z, measurement_t::awz, measurement_ts, false);

            // publish_measurement(imu.gravity.x, measurement_t::gravityx, measurement_ts, false);
            // publish_measurement(imu.gravity.y, measurement_t::gravityy, measurement_ts, false);
            // publish_measurement(imu.gravity.z, measurement_t::gravityz, measurement_ts, false);

            // publish_measurement(imu.qw, measurement_t::qw, measurement_ts, false);
            // publish_measurement(imu.qx, measurement_t::qx, measurement_ts, false);
            // publish_measurement(imu.qy, measurement_t::qy, measurement_ts, false);
            // publish_measurement(imu.qz, measurement_t::qz, measurement_ts, false);
            // vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}