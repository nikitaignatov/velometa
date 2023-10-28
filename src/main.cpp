#include <Arduino.h>
#include "display.h"
#include "queue.h"
#include "hr.h"
#include "power.h"
#include "config.h"

const int screen_height = 128;
const int screen_width = 296;

HR hr_monitor(DEVICE_NAME_HR);
Power power_monitor("KICKR CORE 28DF");

void setup()
{
    Serial.begin(115200);
    Debug("Firmware test starting");
    display_init();
    partial_update(String("display initialized"));

    hr_monitor.init();
    Debug("HR initialized");
    partial_update(String("HR initialized"));

    power_monitor.init();
    Debug("POWER initialized");
    partial_update(String("POWER initialized"));
}

void loop()
{
    long secs = millis() / 1000;
    hr_monitor.loop();
    power_monitor.loop();

    clear();
    display_chart(hr_monitor.queue, 0);
    display_chart(power_monitor.queue, screen_width / 2 + 2);
    render(secs, hr_monitor.heart_rate, power_monitor.power);
}