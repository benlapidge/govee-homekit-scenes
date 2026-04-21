#include "GoveeLight.h"

#include <math.h>

#include "GoveeClient.h"

GoveeLight::GoveeLight(GoveeClient& client, DirectControlCallback onDirectControl)
    : Service::LightBulb(),
      _client(client), _onDirectControl(std::move(onDirectControl)) {
    _on = new Characteristic::On(false);
    _bright = (new Characteristic::Brightness(100))->setRange(1, 100, 1);
    _hue = (new Characteristic::Hue(0))->setRange(0, 360, 1);
    _sat = (new Characteristic::Saturation(0))->setRange(0, 100, 1);
    // Mireds: 140 (~7142K) to 500 (2000K). Govee clamps to 2200-6500K.
    _ct = (new Characteristic::ColorTemperature(250))->setRange(140, 500, 1);

    _stagedBrightness = _bright->getVal<int>();
    _stagedHue = _hue->getVal<float>();
    _stagedSat = _sat->getVal<float>();
    _stagedKelvin = miredsToKelvin(_ct->getVal<int>());
}

boolean GoveeLight::update() {
    bool anyDirect = false;

    if (_on->updated()) {
        _stagedPower = _on->getNewVal<int>() != 0;
        _pendPower = true;
        anyDirect = true;
    }
    if (_bright->updated()) {
        _stagedBrightness = _bright->getNewVal<int>();
        _pendBrightness = true;
        anyDirect = true;
    }
    if (_hue->updated() || _sat->updated()) {
        _stagedHue = _hue->getNewVal<float>();
        _stagedSat = _sat->getNewVal<float>();
        _pendColor = true;
        _pendTemp = false;  // mutually exclusive on device
        _lastModeIsColor = true;
        anyDirect = true;
    }
    if (_ct->updated()) {
        _stagedKelvin = miredsToKelvin(_ct->getNewVal<int>());
        _pendTemp = true;
        _pendColor = false;
        _lastModeIsColor = false;
        anyDirect = true;
    }

    if (anyDirect) {
        _debounce.touch();
        if (_onDirectControl) _onDirectControl();
    }
    return true;
}

void GoveeLight::loop() {
    if (!_debounce.ready()) return;
    dispatchPending();
}

void GoveeLight::dispatchPending() {
    if (_pendPower) {
        _client.setPower(_stagedPower);
        _pendPower = false;
    }
    if (_pendBrightness) {
        _client.setBrightness(_stagedBrightness);
        _pendBrightness = false;
    }
    if (_pendColor && _lastModeIsColor) {
        uint32_t rgb = hsvToRgb(_stagedHue, _stagedSat);
        _client.setColorRgb(rgb);
        _pendColor = false;
        _pendTemp = false;
    } else if (_pendTemp) {
        _client.setColorTempK(_stagedKelvin);
        _pendTemp = false;
        _pendColor = false;
    }
}

// Standard HSV→RGB with V fixed at 1.0; brightness is sent via a separate capability.
uint32_t GoveeLight::hsvToRgb(float h, float s) {
    s = s / 100.0f;
    if (s < 0) s = 0;
    if (s > 1) s = 1;
    float c = s;                     // V * S, with V = 1
    float hh = fmodf(h, 360.0f) / 60.0f;
    float x = c * (1.0f - fabsf(fmodf(hh, 2.0f) - 1.0f));
    float r = 0, g = 0, b = 0;
    if (hh < 1)      { r = c; g = x; }
    else if (hh < 2) { r = x; g = c; }
    else if (hh < 3) { g = c; b = x; }
    else if (hh < 4) { g = x; b = c; }
    else if (hh < 5) { r = x; b = c; }
    else             { r = c; b = x; }
    float m = 1.0f - c;              // V - C, with V = 1
    uint8_t R = static_cast<uint8_t>((r + m) * 255.0f + 0.5f);
    uint8_t G = static_cast<uint8_t>((g + m) * 255.0f + 0.5f);
    uint8_t B = static_cast<uint8_t>((b + m) * 255.0f + 0.5f);
    return (static_cast<uint32_t>(R) << 16) | (static_cast<uint32_t>(G) << 8) | B;
}

uint16_t GoveeLight::miredsToKelvin(uint32_t mireds) {
    if (mireds == 0) return 6500;
    uint32_t k = 1000000UL / mireds;
    if (k < 2200) k = 2200;
    if (k > 6500) k = 6500;
    return static_cast<uint16_t>(k);
}
