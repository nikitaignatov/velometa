#include "button.h"
void Button::setup(uint8_t pin_)
{
    _pin = pin_;
    pinMode(_pin, INPUT_PULLUP);
    digitalWrite(_pin, HIGH);
}

void IRAM_ATTR Button::isr()
{
    long now = millis();
    int new_state = digitalRead(_pin);
    if (now - _last_time > 50 && new_state == LOW && btn_state == HIGH)
    {
        btn++;
        change = true;
        btn_state = new_state;
        _last_time = millis();
    }
    if (now - _last_time > 50 && new_state == HIGH && btn_state == LOW)
    {
        btn_state = new_state;
        _last_time = millis();
    }
}