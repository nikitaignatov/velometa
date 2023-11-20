#include "metric.hpp"

metric_data_t total;

void Metric::reset()
{
    total = {
        .count = 0,
        .sum = 0,
        .period_start = 0,
        .period_end = 0,
        .min = 0,
        .max = 0,
        .avg = 0,
        .last = 0,
    };
}

void Metric::new_reading(metric_t measurement)
{
    auto value = measurement.value;
    total.last = value;
    total.sum += value;
    total.count++;
    total.period_end = measurement.ts;
    if (total.min > value)
        total.min = value;
    if (total.max < value)
        total.max = value;
    total.avg = total.sum / total.count;

    measurements.push_back(measurement);
    printf("%s.count:%d; size:%d; max: %d\n", name, measurements.size(), measurements.capacity(),measurements.max_size());
}

metric_data_t Metric::last()
{
    return total;
}

void metric_task_code(void *parameter)
{
    // Serial.println("sensor_task_code");
    // raw_measurement_msg_t msg;
    // for (;;)
    // {
    //     if (xQueueReceive(vh_raw_measurement_queue, &msg, 1000 / portTICK_RATE_MS) == pdPASS)
    //     {
    //         switch (msg.measurement)
    //         {
    //         case measurement_t::heartrate:
    //             hr_monitor.add_reading(msg.value);
    //             hr_metric.new_reading(msg.value / (float_t)msg.scale);
    //             break;
    //         case measurement_t::power:
    //             power_monitor.add_reading(msg.value);
    //             break;
    //         case measurement_t::speed:
    //             speed_monitor.add_reading(msg.value / msg.scale);
    //             Serial.printf("SPEED\t: %.2f km/h\n", msg.value / (float_t)msg.scale);
    //             break;
    //         case measurement_t::elevation:
    //             Serial.printf("ELEVATION\t: %d meters\n", msg.value / msg.scale);
    //             break;

    //         default:
    //             Serial.printf("Unhandled Message type: %i, Value: %i\n", msg.measurement, msg.value);
    //             break;
    //         }
    //     }
    //     else
    //     {
    //         Serial.println("No items on vh_raw_measurement_queue.");
    //     }
    // }
}