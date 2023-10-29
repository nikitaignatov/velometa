#include <Arduino.h>
#include "queue.h"
#include "hr.h"
#include "power.h"
#include "speed.h"
#include "config.h"
#include "display.h"

const int screen_height = 128;
const int screen_width = 296;

HR hr_monitor(DEVICE_NAME_HR);
Power power_monitor(DEVICE_NAME_POWER);
Speed speed_monitor(DEVICE_NAME_SPEED);

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

    speed_monitor.init();
    Debug("Speed initialized");
    partial_update(String("Speed initialized"));
}

void loop()
{
    long secs = millis() / 1000;
    hr_monitor.loop();
    speed_monitor.loop();
    power_monitor.loop();

    render(secs, &hr_monitor, &power_monitor, &speed_monitor);
}