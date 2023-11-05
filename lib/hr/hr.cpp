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
        Sensor::lastr = millis();

        if (Sensor::minv > Sensor::lastv || Sensor::minv == 0)
            Sensor::minv = Sensor::lastv;
        if (Sensor::maxv < Sensor::lastv)
            Sensor::maxv = Sensor::lastv;
        Sensor::sumv += Sensor::lastv;
        Sensor::count++;
        Sensor::avgv = Sensor::sumv / Sensor::count;
    }
    else
    {
      Sensor::  lastv = 0;
    }

    if (HR_Z1_MIN <Sensor::lastv && HR_Z1_MAX > Sensor::lastv) Sensor::zonev = 1;
    if (HR_Z2_MIN <Sensor::lastv && HR_Z2_MAX > Sensor::lastv) Sensor::zonev = 2;
    if (HR_Z3_MIN <Sensor::lastv && HR_Z3_MAX > Sensor::lastv) Sensor::zonev = 3;
    if (HR_Z4_MIN <Sensor::lastv && HR_Z4_MAX > Sensor::lastv) Sensor::zonev = 4;
    if (HR_Z5_MIN <Sensor::lastv && HR_Z5_MAX > Sensor::lastv) Sensor::zonev = 5;
}

