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
int x_offset = 20;
int y_offset = 90;
bool y = false;
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
            y = !y;
        }
        currentStateCLK = digitalRead(CLK);
        if (currentStateCLK != lastStateCLK && currentStateCLK == 1)
        {
            if (digitalRead(DT) != currentStateCLK)
            {
                counter--;
                currentDir = "CCW";
                if (y)
                    y_offset--;
                else
                    x_offset--;
            }
            else
            {
                counter++;
                currentDir = "CW";

                if (y)
                    y_offset++;
                else
                    x_offset++;
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
    long last = 0;
    for (;;)
    {
        long secs = millis();
        if (refresh)
        {
            // refresh_screen();
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
                // if (mode == HIGH)
                // {
                //     Serial.print(p);
                //     Serial.println("=HIGH");
                // }
                // if (mode == LOW)
                // {
                //     Serial.print(p);
                //     Serial.println("=LOW");
                // }
            }

            // Serial.print("BTN=");
            // Serial.println(sw.btn);
            // Serial.print(" Axis=");
            // Serial.print(y ? "y" : "x");
            // Serial.print(" X=");
            // Serial.print(x_offset);
            // Serial.print(" Y=");
            // Serial.println(y_offset);
        }
        if (secs > last+500)
        {
            // if (x_offset < 100)
            //     x_offset += 4;
            // else
            //     x_offset = 0;
            if (y_offset > 0)
                y_offset -= 2;
            else
                y_offset = 200;
            change = true;
            render(secs/1000, &hr_monitor, &power_monitor, &speed_monitor, counter);
            display_map_dark(x_offset, y_offset);
        }
        last = secs;
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
        10 * 1024,       /* Stack size in words */
        NULL,            /* Task input parameter */
        0,               /* Priority of the task */
        &ble_task,       /* Task handle. */
        0);              /* Core where the task should run */
}

void loop()
{
}