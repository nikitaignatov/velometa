#include "airspeed.hpp"

float Airspeed::inlet(float input)
{
    _inlet = input;
    return _inlet;
}

float Airspeed::throat(float input)
{
    _throat = input;
    return _throat;
}

float Airspeed::pressure(float input)
{
    _pressure = input;
    return pressure();
}

float Airspeed::pressure()
{
    return _pressure;
}

float Airspeed::humidity(float input)
{
    _humidity = input;
    return humidity();
}

float Airspeed::humidity()
{
    return _humidity;
}

float Airspeed::temperature(float input)
{
    _temperature = input;
    return temperature();
}

float Airspeed::temperature()
{
    return _temperature;
}

float Airspeed::density()
{
    float T = temperature(); // Temperature in deg C
    float h = humidity();    // Relative Humidity
    float Pa = pressure();   // Pascals

    float k = 273.13;
    float Rd = 287.058; // gas constant for dry air, J/(kg*degK)
    float Rv = 461.495; //	gas constant for water vapor, J/(kg*degK)

    float Tk = T + k;
    float a = 7.5 * T;
    float b = T + 237.3;
    float Pvs = 6.1078 * 10 * (a / b);
    float Pva = Pvs * h; // pressure of water vapor (partial pressure), Pascals
    float Pd = Pa - Pva; // pressure of dry air (partial pressure), Pascals
    float x = Pd / Rd * Tk;
    float y = Pva / Rv * Tk;
    float p = x + y;      // total air pressure, Pascals ( multiply mb by 100 to get Pascals)
    return p / (Rd * Tk); // kg/m3
}

float Airspeed::speed(float differential_pressure)
{
    float h = 0.010;
    float inlet = h * (_inlet / 100);
    float throat = h * (_throat / 100);
    float ratio = inlet / throat;
    float factor = ((pow(ratio, 2) - 1) * density());
    if (differential_pressure == 0)
        return 0;
    else if (differential_pressure < 0)
        return sqrt((2 * -differential_pressure) / factor);
    else
        return -sqrt(2 * (differential_pressure) / factor);
}

float Airspeed::altitude()
{
    return 0.0f;
}
