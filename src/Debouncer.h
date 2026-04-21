#pragma once

#include <Arduino.h>

class Debouncer {
public:
    explicit Debouncer(uint32_t delayMs) : _delay(delayMs) {}

    void touch() {
        _lastTouch = millis();
        _armed = true;
    }

    bool ready() {
        if (!_armed) return false;
        if (millis() - _lastTouch < _delay) return false;
        _armed = false;
        return true;
    }

    bool armed() const { return _armed; }

private:
    uint32_t _delay;
    uint32_t _lastTouch = 0;
    bool _armed = false;
};
