class Airspeed
{
private:
    float _pressure, _humidity, _temperature, _density, _altitude;

public:
    float pressure(float input);
    float pressure();
    float humidity(float input);
    float humidity();
    float temperature(float input);
    float temperature();
    float density();
    float altitude();
};