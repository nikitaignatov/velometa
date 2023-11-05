#include "power.h"
#include "zones.h"

void Sensor::interpret(uint8_t *pData, size_t length)
{
    // Configure the Cycle Power Measurement characteristic

    // Properties = Notify
    // Min Len    = 2
    // Max Len    = 34
    //    B0:1    = UINT16  - Flag (MANDATORY)
    //      b0    = Pedal power balance present; 0 = false, 1 = true
    //      b1    = Pedal power balance reference; 0 = unknown, 1 = left
    //      b2    = Accumulated torque present; 0 = false, 1 = true
    //      b3    = Accumulated torque source; 0 = wheel, 1 = crank
    //      b4    = Wheel revolution data present; 0 = false, 1 = true
    //      b5    = Crank revolution data present; 0 = false, 1 = true
    //      b6    = Extreme force magnatudes present; 0 = false, 1 = true
    //      b7    = Extreme torque magnatues present; 0 = false, 1 = true
    //      b8    = Extreme angles present; 0 = false, 1 = true
    //      b9    = Top dead angle present; 0 = false, 1 = true
    //      b10   = Bottom dead angle present; 0 = false, 1 = true
    //      b11   = Accumulated energy present; 0 = false, 1 = true
    //      b12   = Offset compensation indicator; 0 = false, 1 = true
    //      b13   = Reseved
    //      b14   = n/a
    //      b15   = n/a
    //    B2:3    = SINT16 - Instataineous power, Watts (decimal)
    //    B4      = UINT8 -  Pedal power balance, Percent (binary) 1/2
    //    B5:6    = UINT16 - Accumulated torque, Nm; res (binary) 1/32
    //    B7:10   = UINT32 - Cumulative wheel revolutions, (decimal)
    //    B11:12  = UINT16 - Last wheel event time, second (binary) 1/2048
    //    B13:14  = UINT16 - Cumulative crank revolutions, (decimal)
    //    B15:16  = UINT16 - Last crank event time, second (binary) 1/1024
    //    B17:18  = SINT16 - Max force magnitude, Newton (decimal)
    //    B19:20  = SINT16 - Min force magnitude, Newton (decimal)
    //    B21:22  = SINT16 - Max torque magnitude, Nm (binary) 1/1024
    //    B23:24  = SINT16 - Min torque magnitude, Nm (binary) 1/1024
    //    B25:26  = UINT12 - Max angle, degree (decimal)
    //    B27:28  = UINT12 - Min angle, degree (decimal)
    //    B29:30  = UINT16 - Top dead spot angle, degree (decimal)
    //    B31:32  = UINT16 - Bottom dead spot angle, degree (decimal)
    //    B33:34  = UINT16 - Accumulated energy, kJ (decimal)

    if (length < 2)
    {
        printf("power data length should be at least 2, received %lu", length);
    }
    if (length > 34)
    {
        printf("power data length should be at most 34, received %lu", length);
    }

    uint16_t flags = (pData[1] << 8) | pData[0];

    int power = (pData[3] << 8) | pData[2];
    uint8_t power_balance = pData[4] / 2;
    int crank = ((pData[6] << 8) | pData[5]);
    int ct = ((pData[8] << 8) | pData[7]) / 1024;

    if (power > 0)
    {
        Sensor::lastv = power;
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
        Sensor::lastv = 0;
    }
    Sensor::zonev = calculate_power_zone(Sensor::lastv);
}