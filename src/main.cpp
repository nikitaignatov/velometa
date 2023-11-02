#include <Arduino.h>
#include "queue.h"
#include "hr.h"
#include "power.h"
#include "speed.h"
#include "config.h"
#include "display_420.h"
#include "button.h"

TaskHandle_t input_task;
TaskHandle_t display_task;
TaskHandle_t ble_task;
void input_task_code(void *parameter);
void display_task_code(void *parameter);
void ble_task_code(void *parameter);

int counter = 0;
bool change = false;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;

const int screen_height = 128;
const int screen_width = 296;
bool refresh = false;

HR hr_monitor(DEVICE_NAME_HR);
Power power_monitor(DEVICE_NAME_POWER);
Speed speed_monitor(DEVICE_NAME_SPEED);

Button sw;
Button rot;

int SW = 2;
int DT = 27;
int CLK = 14;

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
        currentStateCLK = digitalRead(CLK);
        if (currentStateCLK != lastStateCLK && currentStateCLK == 1)
        {
            if (digitalRead(DT) != currentStateCLK)
            {
                counter--;
                currentDir = "CCW";
            }
            else
            {
                counter++;
                currentDir = "CW";
            }

            Serial.print("Direction: ");
            Serial.print(currentDir);
            Serial.print(" | Counter: ");
            Serial.println(counter);

            change = true;
            // partial_update(String("Direction: ") + String(currentDir)+String(" | Counter: ") + String(counter));
        }

        lastStateCLK = currentStateCLK;

        int btnState = digitalRead(SW);

        if (btnState == LOW)
        {
            // if 50ms have passed since last LOW pulse, it means that the
            // button has been pressed, released and pressed again
            if (millis() - lastButtonPress > 50)
            {
                Serial.println("Button pressed!");
            }

            // Remember last button press event
            lastButtonPress = millis();
        }
        delay(1);
    }
}

void display_task_code(void *parameter)
{
    Serial.println("display_task_code");
    for (;;)
    {
        long secs = millis() / 1000;
        render(secs, &hr_monitor, &power_monitor, &speed_monitor);
        if (refresh)
        {
            refresh_screen();
            refresh = false;
        }
        if (sw.change || change)
        {
            sw.change = false;
            change = false;
            int k[3] = {0, 2, 15};
            for (size_t i = 0; i < 3; i++)
            {
                int p = k[i];
                int mode = digitalRead(p);
                if (mode == HIGH)
                {
                    Serial.print(p);
                    Serial.println("=HIGH");
                }
                if (mode == LOW)
                {
                    Serial.print(p);
                    Serial.println("=LOW");
                }
            }

            Serial.print("BTN=");
            Serial.println(sw.btn);
            partial_update(String("BTN=") + String(sw.btn) + String("ROT=") + String(counter) + String("DIR=") + currentDir);
        }
        delay(50);
    }
}
void ble_task_code(void *parameter)
{
    Serial.println("ble_task_code");
    hr_monitor.init();
    // partial_update(String("HR.init"));

    power_monitor.init();
    // partial_update(String("POWER.init"));

    speed_monitor.init();
    // partial_update(String("Speed.init"));
    Serial.println("ble_task_code setup done.");
    for (;;)
    {
        hr_monitor.loop();
        speed_monitor.loop();
        power_monitor.loop();
        delay(1000);
    }
}
void setup()
{
    Serial.begin(115200);
    display_init();
    partial_update(String("display initialized"));
    sw.setup(SW);

    pinMode(DT, INPUT);
    pinMode(CLK, INPUT);
    lastStateCLK = digitalRead(CLK);

    attachInterrupt(
        SW, []()
        { sw.isr(); },
        CHANGE);

    xTaskCreatePinnedToCore(
        input_task_code,   /* Function to implement the task */
        "input_task_code", /* Name of the task */
        8 * 1024,          /* Stack size in words */
        NULL,              /* Task input parameter */
        0,                 /* Priority of the task */
        &input_task,       /* Task handle. */
        0);                /* Core where the task should run */

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
        10 * 1024,       /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &ble_task,       /* Task handle. */
        0);              /* Core where the task should run */
}

void loop()
{
}