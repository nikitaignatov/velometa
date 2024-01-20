#include <math.h>

class Airspeed
{
private:
    float _pressure, _humidity, _temperature, _density, _altitude,_inlet,_throat;

public:
    float inlet(float input);
    float throat(float input);
    float pressure(float input);
    float pressure();
    float humidity(float input);
    float humidity();
    float temperature(float input);
    float temperature();
    float density();
    float altitude();
    float speed(float differential_pressure);
};