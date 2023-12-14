#include "activity.hpp"

static const char *TAG = "activity_task_code";
static Activity activity;
static ride_data_t telemetry;

static metric_info_t new_reading(metric_info_t total, raw_measurement_msg_t measurement)
{
    auto value = measurement.value;
    total.last = value;
    total.sum += value;
    total.count++;
    total.min = std::min(total.min, (float)value);
    total.max = std::max(total.max, (float)value);
    total.avg = total.sum / total.count;
    return total;
}

void Activity::init()
{
    auto size = RIDE_HOURS_MAX * H_SECONDS;

    telemetry = (ride_data_t){
        .lat = (float *)ps_malloc(size * sizeof(float)),
        .lon = (float *)ps_malloc(size * sizeof(float)),
        .elevation = (uint16_t *)ps_malloc(size * sizeof(uint16_t)),
        .pressure = (uint16_t *)ps_malloc(size * sizeof(uint16_t)),
        .power = (uint16_t *)ps_malloc(size * sizeof(uint16_t)),
        .speed = (uint16_t *)ps_malloc(size * sizeof(uint16_t)),
        .hr = (uint8_t *)ps_malloc(size * sizeof(uint8_t)),
        .cadence = (uint8_t *)ps_malloc(size * sizeof(uint8_t)),
        .slope = (uint8_t *)ps_malloc(size * sizeof(uint8_t)),
    };
}
void Activity::set_tick(uint16_t seconds)
{
    this->seconds = seconds;
}

metric_info_t Activity::get_hr() { return hr; }
metric_info_t Activity::get_power() { return power; }
metric_info_t Activity::get_speed() { return speed; }

void Activity::add_measurement(raw_measurement_msg_t msg)
{
    auto index = msg.ts / H_SECONDS;
    ESP_LOGD(TAG, "activity::add_measurement hour[%d]", hour);
    switch (msg.measurement)
    {
    case measurement_t::heartrate:
        telemetry.hr[seconds] = msg.value;
        hr = new_reading(hr, msg);
        for (auto &interval : this->counters)
        {
            auto index = std::max(0, seconds - interval.count);
            uint16_t old = telemetry.hr[index];
            interval.add(&interval, msg, old);
            ESP_LOGW(TAG, "period %ds = %d | %d | %d | %d | %d", interval.duration, interval.avg, interval.count, interval.sum, seconds, index);
        }
        return;
    case measurement_t::power:
        telemetry.power[seconds] = msg.value;
        power = new_reading(power, msg);
        return;
    case measurement_t::speed:
        telemetry.speed[seconds] = msg.value;
        speed = new_reading(speed, msg);
        return;
    default:
        break;
    }
}

void activity_task_code(void *parameter)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_LOGI(TAG, "start task setup");
    activity.init();
    ESP_LOGI(TAG, "end task setup");
    raw_measurement_msg_t msg;
    for (;;)
    {
        if (xQueueReceive(vh_raw_measurement_queue, &msg, 1000 / portTICK_RATE_MS) == pdPASS)
        {
            ESP_LOGI(TAG, "start xQueueReceive");
            ESP_LOGI(TAG, "start add_measurement");
            activity.add_measurement(msg);
            ESP_LOGI(TAG, "end add_measurement");

            switch (msg.measurement)
            {
            case measurement_t::heartrate:
                publish(MSG_NEW_HR, activity.get_hr());
                break;
            case measurement_t::power:
                publish(MSG_NEW_POWER, activity.get_power());
                break;
            case measurement_t::speed:
                publish(MSG_NEW_SPEED, activity.get_speed());
                break;
            case measurement_t::elevation:
                break;
            default:
                ESP_LOGI(TAG, "Unhandled Message type: %i, Value: %i\n", msg.measurement, msg.value);
                break;
            }
            ESP_LOGI(TAG, "end xQueueReceive");
        }
        else
        {
            ESP_LOGI(TAG, "No items on vh_raw_measurement_queue.");
        }
    }
}