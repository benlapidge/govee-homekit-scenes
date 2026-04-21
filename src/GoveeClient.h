#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class Config;

class GoveeClient {
public:
    explicit GoveeClient(Config& cfg) : _cfg(cfg) {}

    bool setPower(bool on);
    bool setBrightness(uint8_t pct);          // 1-100
    bool setColorRgb(uint32_t rgb);           // 0x00RRGGBB
    bool setColorTempK(uint16_t kelvin);      // 2200-6500
    bool setScene(uint32_t paramId, uint32_t id);

    // TODO: state reconciliation — periodic poll of device/state to resync HomeKit.
    bool pollDeviceState() { return false; }

private:
    bool postCapability(const char* type, const char* instance,
                        JsonVariantConst value);

    Config& _cfg;
};
