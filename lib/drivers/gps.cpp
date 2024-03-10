#include "gps.hpp"

#if BOARD == BOARD_LILY_154
static const int RXPin = 13, TXPin = 26;
#elif BOARD == BOARD_LILY_WRIST
static const int RXPin = 21, TXPin = 22;
#elif BOARD == WTSC01_PLUS
static const int RXPin = 11, TXPin = 10;
#endif
static const uint32_t GPSBaud = 115200;
TinyGPSPlus gps;
HardwareSerial ss(1);


void gps_task_code(void *parameter)
{
    Serial.println("gps_task_code");
    pinMode(RXPin, INPUT);
    pinMode(TXPin, OUTPUT);
    ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);

    Serial.println("gps_task_code setup done.");
    int time = 0;
    for (;;)
    {
        while (ss.available() > 0)
        {
            if (gps.encode(ss.read()))
            {

                gps_data_t data;
                if (!vh_raw_measurement_queue)
                {
                    delay(1000);
                    Serial.println("gps_task_code queue is null");
                    continue;
                }

                if (gps.time.value() == time)
                {
                    continue;
                }
                time = gps.time.value();

                raw_measurement_msg_t msg = {
                    .measurement = measurement_t::speed,
                    .ts = gps.date.value() * 1000,
                    .value = gps.speed.kmph(),
                };
                xQueueSend(vh_raw_measurement_queue, &msg, 0);

                msg = {
                    .measurement = measurement_t::elevation,
                    .ts = gps.date.value() * 1000,
                    .value = gps.altitude.meters(),
                };
                xQueueSend(vh_raw_measurement_queue, &msg, 0);

                data = (gps_data_t){
                    .tick_ms = gps.time.value(),
                    .date = gps.date.value(),
                    .time = gps.time.value(),
                    .lat = gps.location.lat(),
                    .lon = gps.location.lng(),
                    .speed = gps.speed.kmph(),
                    .height = gps.altitude.meters(),
                    .has_fix = gps.sentencesWithFix(),
                    .satelites = gps.satellites.value(),
                };

                // delay(1000);
                // continue;

                Serial.print(F("Location: "));

                if (gps.location.isValid())
                {
                    Serial.print(gps.location.lat(), 6);
                    Serial.print(F(","));
                    Serial.print(gps.location.lng(), 6);
                }
                else
                {
                    Serial.print(F("INVALID"));
                }

                Serial.print(F("  Date.value[ "));
                Serial.print(gps.date.value());
                Serial.print(F(" ] "));
                Serial.print(F("  Time.value[ "));
                Serial.print(gps.time.value());
                Serial.print(F(" ] "));
                Serial.print(F("  Date/Time: "));
                if (gps.date.isValid())
                {
                    Serial.print(gps.date.year());
                    Serial.print(F("-"));
                    Serial.print(gps.date.month());
                    Serial.print(F("-"));
                    Serial.print(gps.date.day());
                }
                else
                {
                    Serial.print(F("INVALID"));
                }

                Serial.print(F("T"));
                if (gps.time.isValid())
                {
                    if (gps.time.hour() < 10)
                        Serial.print(F("0"));
                    Serial.print(gps.time.hour());
                    Serial.print(F(":"));
                    if (gps.time.minute() < 10)
                        Serial.print(F("0"));
                    Serial.print(gps.time.minute());
                    Serial.print(F(":"));
                    if (gps.time.second() < 10)
                        Serial.print(F("0"));
                    Serial.print(gps.time.second());
                    Serial.print(F("."));
                    if (gps.time.centisecond() < 10)
                        Serial.print(F("0"));
                    Serial.print(gps.time.centisecond());
                }
                else
                {
                    Serial.print(F("INVALID"));
                }

                Serial.println("Z");
                // delay(1000);
            }
        }

        // if (millis() > 5000 && gps.charsProcessed() < 10)
        // {
        //     Serial.println(F("No GPS detected: check wiring."));
        // }
        // delay(1000);
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