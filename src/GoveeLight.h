#pragma once

#include <HomeSpan.h>

#include <functional>

#include "Debouncer.h"

class GoveeClient;

class GoveeLight : public Service::LightBulb {
public:
    using DirectControlCallback = std::function<void()>;

    GoveeLight(GoveeClient& client, DirectControlCallback onDirectControl);

    boolean update() override;
    void loop() override;

    static uint32_t hsvToRgb(float h, float s);          // V fixed at 1.0
    static uint16_t miredsToKelvin(uint32_t mireds);     // clamped 2200-6500

private:
    void dispatchPending();

    GoveeClient& _client;
    DirectControlCallback _onDirectControl;

    SpanCharacteristic* _on;
    SpanCharacteristic* _bright;
    SpanCharacteristic* _hue;
    SpanCharacteristic* _sat;
    SpanCharacteristic* _ct;

    Debouncer _debounce{300};

    bool _pendPower = false;
    bool _pendBrightness = false;
    bool _pendColor = false;
    bool _pendTemp = false;

    bool _stagedPower = false;
    uint8_t _stagedBrightness = 100;
    float _stagedHue = 0;
    float _stagedSat = 0;
    uint16_t _stagedKelvin = 4000;

    // True if colour was the most recently touched mode-defining characteristic.
    // Resolves the colour-vs-temp exclusivity if both arrive in one debounce window.
    bool _lastModeIsColor = true;
};
