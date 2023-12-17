#include "activity.hpp"

static const char *TAG = "activity_task_code";
static Activity activity;
static ride_data_t telemetry;

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

window_counter_t Activity::get_hr(uint16_t duration) { return counters[measurement_t::heartrate].at(0); }
window_counter_t Activity::get_hr() { return counters[measurement_t::heartrate].at(8); }
window_counter_t Activity::get_power() { return counters[measurement_t::power].at(8); }
window_counter_t Activity::get_speed() { return counters[measurement_t::speed].at(8); }

void Activity::add_measurement(raw_measurement_msg_t msg)
{
    ESP_LOGD(TAG, "activity::add_measurement hour[%d]", hour);

    for (auto &interval : this->counters[msg.measurement])
    {
        interval.add(&interval, (uint16_t)msg.value, (uint16_t *)&telemetry.hr[interval.window_start]);
        ESP_LOGD(TAG, "period %ds \t avg: %d \t count: %d \tfrom: %d \tto: %d", interval.duration, interval.avg, interval.count, interval.window_end - interval.duration, seconds);
    }
    switch (msg.measurement)
    {
    case measurement_t::heartrate:
        telemetry.hr[seconds] = msg.value;
        return;
    case measurement_t::power:
        telemetry.power[seconds] = msg.value;
        return;
    case measurement_t::speed:
        telemetry.speed[seconds] = msg.value;
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
        vTaskDelay(100 / portTICK_PERIOD_MS);

        auto seconds = (uint16_t)millis() / 1000;
        activity.set_tick(seconds);
        while (xQueueReceive(vh_raw_measurement_queue, &msg, 5 / portTICK_RATE_MS) == pdPASS)
        {
            ESP_LOGD(TAG, "start xQueueReceive");
            ESP_LOGD(TAG, "start add_measurement");
            activity.add_measurement(msg);
            ESP_LOGD(TAG, "end add_measurement");

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
                ESP_LOGW(TAG, "Unhandled Message type: %i, Value: %i\n", msg.measurement, msg.value);
                break;
            }
            ESP_LOGD(TAG, "end xQueueReceive");
        }
    }
}