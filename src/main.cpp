#include <Arduino.h>
#include "queue.h"
#include "hr.h"
#include "power.h"
#include "speed.h"
#include "config.h"
#include "display_420.h"
#include "button.h"
#include <TinyGPSPlus.h>

#include "gps.h"

static const int RXPin = 13, TXPin = 26;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
HardwareSerial ss(1);

float_t speed;
float_t lat, lon, height;

TaskHandle_t input_task;
TaskHandle_t display_task;
TaskHandle_t ble_task;
TaskHandle_t gps_task;

void input_task_code(void *parameter);
void display_task_code(void *parameter);
void ble_task_code(void *parameter);
void gps_task_code(void *parameter);

const int screen_height = 300;
const int screen_width = 400;
bool refresh = false;

HR hr_monitor(DEVICE_NAME_HR, 400);
Power power_monitor(DEVICE_NAME_POWER, 400);
Speed speed_monitor(DEVICE_NAME_SPEED, 10);

Button sw;

int SW = 2;

void input_task_code(void *parameter)
{
    Serial.println("input_task_code");
    for (;;)
    {
        if (sw.change)
        {
            refresh = true;
            sw.change = false;
        }
        delay(1);
    }
}

void display_task_code(void *parameter)
{
    Serial.println("display_task_code");
    long last = 0;
    for (;;)
    {
        long secs = millis();
        if (refresh)
        {
            refresh_screen();
            refresh = false;
        }
        render(secs / 1000, &hr_monitor, &power_monitor, &speed_monitor);
        display_bottom(height, speed, lat, lon);
        show();
    }
}

void ble_task_code(void *parameter)
{
    Serial.println("ble_task_code");
    hr_monitor.init();
    power_monitor.init();
    speed_monitor.init();
    Serial.println("ble_task_code setup done.");
    for (;;)
    {
        hr_monitor.loop();
        speed_monitor.loop();
        power_monitor.loop();
        Serial.println("ble_task_code loop done.");
        delay(1000);
    }
}

void gps_task_code(void *parameter)
{
    Serial.println("gps_task_code");
    ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
    Serial.println("gps_task_code setup done.");
    for (;;)
    {
        while (ss.available() > 0)
            if (gps.encode(ss.read()))
            {
                height = gps.altitude.meters();
                speed = gps.speed.kmph();
                lat = gps.location.lat();
                lon = gps.location.lng();
                pixel_t p = convert_geo_to_pixel(lat, lon, 1085, 762, 12.170583828160401, (12.186787243525105 - 12.170583828160401), 55.7772468557264, 55.7772468557264 * M_PI / 180);

                Serial.print("X:");
                Serial.print(p.x);
                Serial.print("Y:");
                Serial.println(p.y);
                lat = p.x;
                lon = p.y;

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

                if (millis() > 5000 && gps.charsProcessed() < 10)
                {
                    Serial.println(F("No GPS detected: check wiring."));
                }
                delay(1);
            }
    }
}

void setup()
{
    Serial.begin(115200);
    display_init();
    partial_update(String("display initialized"));
    sw.setup(SW);

    attachInterrupt(
        SW, []()
        { sw.isr(); },
        CHANGE);

    // xTaskCreatePinnedToCore(
    //     input_task_code,   /* Function to implement the task */
    //     "input_task_code", /* Name of the task */
    //     8 * 1024,          /* Stack size in words */
    //     NULL,              /* Task input parameter */
    //     0,                 /* Priority of the task */
    //     &input_task,       /* Task handle. */
    //     0);                /* Core where the task should run */

    xTaskCreatePinnedToCore(
        display_task_code,   /* Function to implement the task */
        "display_task_code", /* Name of the task */
        8 * 1024,            /* Stack size in words */
        NULL,                /* Task input parameter */
        0,                   /* Priority of the task */
        &display_task,       /* Task handle. */
        0);                  /* Core where the task should run */

    xTaskCreatePinnedToCore(
        ble_task_code,   /* Function to implement the task */
        "ble_task_code", /* Name of the task */
        16 * 1024,       /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &ble_task,       /* Task handle. */
        0);              /* Core where the task should run */

    xTaskCreatePinnedToCore(
        gps_task_code,   /* Function to implement the task */
        "gps_task_code", /* Name of the task */
        4 * 1024,        /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &gps_task,       /* Task handle. */
        0);              /* Core where the task should run */
}

void loop()
{
}