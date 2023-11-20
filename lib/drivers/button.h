#include <Arduino.h>

class Button
{
private:
    static const uint32_t DEBOUNCE_DELAY_MILLIS = 50;

    uint8_t _pin;
    uint32_t _last_time;

    int btna = 0;
    int btn_state = HIGH;

public:
    int btn = 0;
    bool change = false;

    void setup(uint8_t pin_);
    void IRAM_ATTR isr();
};