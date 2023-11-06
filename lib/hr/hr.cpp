#include "hr.h"

void Sensor::interpret(uint8_t *pData, size_t length)
{
    int hr = 0;
    int i = 0;

    uint8_t byte0 = pData[0];
    boolean hrv_uint8 = (byte0 & 1) == 0;
    uint8_t sensor_contact = (byte0 >> 1) & 3;
    if (sensor_contact == 2)
    {
        Serial.println("No contact detected");
    }
    else if (sensor_contact == 3)
    {
        //        contact = "Contact detected";
    }
    else
    {
        Serial.println("Sensor contact not supported");
    }

    if (hrv_uint8)
    {
        hr = pData[1];
        i = 2;
    }
    else
    {
        hr = (pData[2] << 8) | pData[1];
        i = 3;
    }

    if (hr > 0)
    {
        Sensor::lastv = hr;
    }
    else
    {
        Sensor::lastv = 0;
    }
    Sensor::zonev = calculate_hr_zone(Sensor::lastv);
}
