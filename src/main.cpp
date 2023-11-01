#include <Arduino.h>
#include "queue.h"
#include "hr.h"
#include "power.h"
#include "speed.h"
#include "config.h"
#include "display_420.h"
#include "button.h"

const int screen_height = 128;
const int screen_width = 296;

HR hr_monitor(DEVICE_NAME_HR);
Power power_monitor(DEVICE_NAME_POWER);
Speed speed_monitor(DEVICE_NAME_SPEED);

Button sw;
Button rot;

int SW = 2;
int DT = 27;
int CLK = 25;

void setup()
{
    Serial.begin(115200);
    display_init();
    partial_update(String("display initialized"));

    hr_monitor.init();
    partial_update(String("HR.init"));

    power_monitor.init();
    partial_update(String("POWER.init"));

    speed_monitor.init();
    partial_update(String("Speed.init"));
    sw.setup(SW);
    rot.setup(DT);

    pinMode(DT, INPUT);
    pinMode(CLK, INPUT);
    attachInterrupt(
        SW, []()
        { sw.isr(); },
        CHANGE);
    attachInterrupt(
        DT, []()
        { rot.isr(); },
        CHANGE);
}

void loop()
{
    long secs = millis() / 1000;
    hr_monitor.loop();
    speed_monitor.loop();
    power_monitor.loop();

    render(secs, &hr_monitor, &power_monitor, &speed_monitor);
    // if (sw.change||rot.change)
    // {
    //     sw.change = false;
    //     int k[3] = {0, 2, 15};
    //     for (size_t i = 0; i < 3; i++)
    //     {
    //         int p = k[i];
    //         int mode = digitalRead(p);
    //         if (mode == HIGH)
    //         {
    //             Serial.print(p);
    //             Serial.println("=HIGH");
    //         }
    //         if (mode == LOW)
    //         {
    //             Serial.print(p);
    //             Serial.println("=LOW");
    //         }
    //     }

    //     Serial.print("BTN=");
    //     Serial.println(sw.btn);
    //     partial_update(String("BTN=") + String(sw.btn)+String("ROT=") + String(rot.btn));
    //     delay(50);
    // }
}