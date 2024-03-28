#include "gps.hpp"

#if BOARD == BOARD_LILY_154
static const int RXPin = 13, TXPin = 26;
#elif BOARD == BOARD_LILY_WRIST
static const int RXPin = 21, TXPin = 22;
#elif BOARD == WTSC01_PLUS
static const int RXPin = 11, TXPin = 10;
#endif
static const uint32_t GPSBaud = 115200;
// TinyGPSPlus gps;
HardwareSerial ss(1);

static const char *TAG = "gps";

static NMEAGPS gps;
static gps_fix fix;

uint64_t ts()
{
    NeoGPS::clock_t seconds = fix.dateTime;
    uint64_t ts = (seconds + (uint64_t)946684800) * (uint64_t)1000;
    auto ms = gps.UTCms();
    auto us = gps.UTCus();
    ts = ts + ms;
    return ts;
}

void gps_task_code(void *parameter)
{
    ESP_LOGI(TAG, "gps_task_code");
    pinMode(RXPin, INPUT);
    pinMode(TXPin, OUTPUT);
    // ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
    gpsPort.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);

    // gps.send_P(&gpsPort, F("PMTK220,1000"));
    ESP_LOGI(TAG, "gps_task_code setup done.");
    int time = 0;
    NeoGPS::Location_t prev(55.0f, 12.0f); // Ayers Rock, AU

    for (;;)
    {
        // ESP_LOGI(TAG, "gps_task_code setup done.");
        // while (gpsPort.available())
        // {
        //     ESP_LOGI(TAG, "%s", gpsPort.readStringUntil('\n').c_str());
        // }
        // continue;

        while (gps.available(gpsPort))
        {
            fix = gps.read();

            if (fix.valid.location)
            {
                NeoGPS::clock_t seconds = fix.dateTime;
                uint64_t ts = (seconds + (uint64_t)946684800) * (uint64_t)1000;
                auto ms = gps.UTCms();
                auto us = gps.UTCus();
                ts = ts + ms;

                ESP_LOGI(TAG, "%llu: %0.6f,%0.6f,%d,%0.2f,%0.2f,%d,%d,%d", ts, fix.latitude(), fix.longitude(), fix.satellites, fix.speed_kph(), fix.heading(), fix.altitude_cm(), fix.hdop, fix.vdop, fix.location.DistanceKm(prev));
                prev = fix.location;

                raw_measurement_msg_t msg = {
                    .measurement = measurement_t::speed,
                    .ts = ts,
                    .value = fix.speed_kph(),
                };
                xQueueSend(vh_raw_measurement_queue, &msg, 0);

                // msg = {
                //     .measurement = measurement_t::elevation,
                //     .ts = gps.date.value() * 1000,
                //     .value = gps.altitude.meters(),
                // };
                // // xQueueSend(vh_raw_measurement_queue, &msg, 0);
                auto data = (gps_data_t){
                    .tick_ms = ts,
                    .lat = fix.latitude(),
                    .lon = fix.longitude(),
                    .speed = fix.speed_kph(),
                    .height = fix.altitude_cm(),
                    .heading = fix.heading(),
                    .distance = fix.location.DistanceKm(prev),
                    .hdop = fix.hdop,
                    .vdop = fix.vdop,
                    .age = gps.UTCms(),
                    .has_fix = fix.valid.location,
                    .satelites = fix.satellites,
                };
                xQueueSend(vh_gps_csv_queue, &data, 0);
                data = (gps_data_t){
                    .tick_ms = ts,
                    .lat = fix.latitude(),
                    .lon = fix.longitude(),
                    .speed = fix.speed_kph(),
                    .height = fix.altitude_cm(),
                    .heading = fix.heading(),
                    .distance = fix.location.DistanceKm(prev),
                    .hdop = fix.hdop,
                    .vdop = fix.vdop,
                    .age = gps.UTCms(),
                    .has_fix = fix.valid.location,
                    .satelites = fix.satellites,
                };
                xQueueSend(vh_gps_queue, &data, 0);
            }
        }
    }
}

void gps_process_task_code(void *parameter)
{
    Serial.println("gps_task_code");
    gps_data_t msg;
    for (;;)
    {
        while (xQueueReceive(vh_gps_queue, &msg, 100 / portTICK_RATE_MS) == pdPASS)
        {
            // todo process gps data
            publish(MSG_NEW_GPS, msg);
        }
    }
}