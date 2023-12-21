#include "activity.hpp"

static const char *TAG = "activity_task_code";
static Activity activity;
static ride_data_t telemetry;

Activity *current_activity()
{
    return &activity;
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
        .hr = (uint16_t *)ps_malloc(size * sizeof(uint16_t)),
        .cadence = (uint16_t *)ps_malloc(size * sizeof(uint16_t)),
        .slope = (uint16_t *)ps_malloc(size * sizeof(uint16_t)),
    };
    memset(telemetry.elevation, 0, size * sizeof(*telemetry.elevation));
    memset(telemetry.pressure, 0, size * sizeof(*telemetry.pressure));
    memset(telemetry.power, 0, size * sizeof(*telemetry.power));
    memset(telemetry.speed, 0, size * sizeof(*telemetry.speed));
    memset(telemetry.hr, 0, size * sizeof(*telemetry.hr));
    memset(telemetry.cadence, 0, size * sizeof(*telemetry.cadence));
    memset(telemetry.slope, 0, size * sizeof(*telemetry.slope));
}

uint16_t Activity::get_tick()
{
    return this->seconds;
}
void Activity::tick()
{
    this->seconds++;
}

void Activity::set_tick(uint16_t seconds)
{
    this->seconds = seconds;
}

void Activity::set_end(uint16_t seconds)
{
    this->ts_end = seconds;
}

void Activity::set_start(uint16_t seconds)
{
    this->ts_start = seconds;
}



window_counter_t Activity::get_hr(uint16_t duration) { return counters[measurement_t::heartrate].at(duration); }
window_counter_t Activity::get_hr() { return counters[measurement_t::heartrate].at(0); }
window_counter_t Activity::get_power() { return counters[measurement_t::power].at(0); }
window_counter_t Activity::get_power(uint16_t duration) { return counters[measurement_t::power].at(duration); }
window_counter_t Activity::get_speed() { return counters[measurement_t::speed].at(0); }

void Activity::add_measurement(raw_measurement_msg_t msg)
{
    ESP_LOGD(TAG, "activity::add_measurement hour[%d]", hour);

    for (auto &interval : this->counters[msg.measurement])
    {
        ESP_LOGD(TAG, "period %ds \t value: %d", interval.duration, msg.value);
        switch (msg.measurement)
        {
        case measurement_t::heartrate:
            interval.add(&interval, (uint16_t)msg.value, telemetry.hr, seconds);
            telemetry.hr[interval.get_position()] = msg.value;
            break;
        case measurement_t::power:
            interval.add(&interval, (uint16_t)msg.value, telemetry.power, seconds);
            telemetry.power[interval.get_position()] = msg.value;
            break;
        case measurement_t::speed:
            interval.add(&interval, (uint16_t)msg.value, telemetry.speed, seconds);
            telemetry.speed[interval.get_position()] = msg.value;
            break;
        default:
            break;
        }
        ESP_LOGD(TAG, "period %ds\t avg: %d count: %d from: %d to: %d", interval.duration, interval.avg, interval.count, interval.window_end - interval.duration, seconds);
    }
}

void update_sec_task(void *parameter)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_LOGI(TAG, "update_sec_task");
    portMUX_TYPE spin_lock;
    spinlock_initialize(&spin_lock);
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1000;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        taskENTER_CRITICAL(&spin_lock);
        activity.tick();
        taskEXIT_CRITICAL(&spin_lock);
    }
}

SemaphoreHandle_t xSemaphore;
void activity_task_code(void *parameter)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_LOGI(TAG, "start task setup");
    activity.init();
    ESP_LOGI(TAG, "end task setup");
    portMUX_TYPE spin_lock;
    spinlock_initialize(&spin_lock);
    raw_measurement_msg_t msg;
    xSemaphore = xSemaphoreCreateMutex();

    if (xSemaphore == NULL)
    {
        ESP_LOGD(TAG, "failed to create semaphore");
    }
    xTaskCreatePinnedToCore(
        update_sec_task,
        "update_sec_task",
        4 * 1024,
        NULL,
        0,
        NULL,
        1);

    for (;;)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        while (xQueueReceive(vh_raw_measurement_queue, &msg, 5 / portTICK_RATE_MS) == pdPASS)
        {
            ESP_LOGD(TAG, "start xQueueReceive");
            ESP_LOGD(TAG, "start add_measurement");
            taskENTER_CRITICAL(&spin_lock);
            if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE)
            {
                activity.add_measurement(msg);
                xSemaphoreGive(xSemaphore);
            }
            taskEXIT_CRITICAL(&spin_lock);
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