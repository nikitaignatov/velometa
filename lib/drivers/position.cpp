#include "position.hpp"

#if BOARD == BOARD_LILY_154
static const int SDA = 12, SCL = 13;
#elif BOARD == BOARD_LILY_WRIST
static const int SDA = 12, SCL = 13;
#elif BOARD == WTSC01_PLUS
static const int SDA2 = 11, SCL2 = 10;
#endif

static const char *TAG = "position";
TwoWire bus(1);
static Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void position_task_code(void *parameter)
{
    ESP_LOGI(TAG, "position_task_code");
    auto count = 0.0;
    Wire.end();
    Wire.begin(SDA2, SCL2);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "VL53L0X test");
    if (!lox.begin())
    {
        ESP_LOGE(TAG, "Failed to boot VL53L0X r");
        while (1)
            ;
    }

    float y = 0.0;
    float a = 0.80;
    for (;;)
    {
        VL53L0X_RangingMeasurementData_t measure;
        auto debug = false;

        lox.rangingTest(&measure, debug);

        if (measure.RangeStatus != 4)
        {
            auto x = measure.RangeMilliMeter;
            y = (a * y) + (x - (a * x));
            ESP_LOGI(TAG, "VL53L0X: %.2f", y);
            raw_measurement_msg_t msg = {
                .measurement = (measurement_t::position_mm),
                .ts = ts(),
                .value = y,
            };
            if (vh_raw_measurement_queue)
            {
                xQueueSend(vh_raw_measurement_queue, &msg, 0);
            }
        }
        else
        {
            ESP_LOGI(TAG, "VL53L0X: Out of range");
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
