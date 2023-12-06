#include "sensor.hpp"

activity_metrics_t activity = {
    .hr = {0, 0, 200, 0, 0, 0, 0, 0, 0},
    .power = {0, 0, 0, 0, 0, 0, 0, 0, 0},
    .speed = {0, 0, 0, 0, 0, 0, 0, 0, 0},
};

uint16_t Sensor::last() { return lastv; }
uint16_t Sensor::avg() { return avgv; }
uint16_t Sensor::min() { return minv; }
uint16_t Sensor::max() { return maxv; }

void Sensor::init() {}

void Sensor::add_reading(uint16_t value)
{
    lastv = value;
    sumv += value;
    count++;
    avgv = sumv / count;
    if (minv > value)
        minv = value;
    if (maxv < value)
        maxv = value;
    enqueue(queue, value);
}

void sensor_task_code(void *parameter)
{
    Serial.println("sensor_task_code");
    raw_measurement_msg_t msg;
    metric_t m;
    metric_info_t n;
    int count = 1;
    for (;;)
    {
        if (xQueueReceive(vh_raw_measurement_queue, &msg, 1000 / portTICK_RATE_MS) == pdPASS)
        {
            switch (msg.measurement)
            {
            case measurement_t::heartrate:
                m = {
                    .ts = 0,
                    .value = (float)msg.value / (float)msg.scale,
                };

                n = update_stats(activity.hr, m);
                activity.hr = n;

                publish(MSG_NEW_HR, n);

                break;
            case measurement_t::power:

                m = {
                    .ts = 0,
                    .value = (float)msg.value / (float)msg.scale,
                };

                n = update_stats(activity.power, m);
                activity.power = n;

                publish(MSG_NEW_POWER, n);

                break;
            case measurement_t::speed:

                m = {
                    .ts = 0,
                    .value = (float)msg.value / (float)msg.scale,
                };
                n = update_stats(activity.speed, m);
                activity.speed = n;

                publish(MSG_NEW_SPEED, n);
                // Serial.printf("SPEED\t: %.2f km/h\n", msg.value / (float_t)msg.scale);
                break;
            case measurement_t::elevation:
                // Serial.printf("ELEVATION\t: %d meters\n", msg.value / msg.scale);
                break;

            default:
                Serial.printf("Unhandled Message type: %i, Value: %i\n", msg.measurement, msg.value);
                break;
            }
        }
        else
        {
            Serial.println("No items on vh_raw_measurement_queue.");
        }
        if (count > 90)
            count = 0;
        // auto p = (metric_info_t){
        //     .ts = millis(),
        //     .last = (float)count++};
        // publish(MSG_NEW_HEADING, p);
    }
}