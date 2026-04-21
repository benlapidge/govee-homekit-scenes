#include "Config.h"

#include <Preferences.h>

namespace {
constexpr const char* NS = "govee";
constexpr const char* KEY_API = "apikey";
constexpr const char* KEY_DEVICE = "device";
constexpr const char* KEY_SKU = "sku";

constexpr const char* DEFAULT_DEVICE = "1F:09:DE:B6:40:86:50:58";
constexpr const char* DEFAULT_SKU = "H6076";
}

void Config::load() {
    Preferences p;
    p.begin(NS, true);
    _apiKey = p.getString(KEY_API, "");
    _device = p.getString(KEY_DEVICE, DEFAULT_DEVICE);
    _sku = p.getString(KEY_SKU, DEFAULT_SKU);
    p.end();
}

void Config::setApiKey(const String& v) {
    _apiKey = v;
    Preferences p;
    p.begin(NS, false);
    p.putString(KEY_API, v);
    p.end();
}

void Config::setDevice(const String& v) {
    _device = v;
    Preferences p;
    p.begin(NS, false);
    p.putString(KEY_DEVICE, v);
    p.end();
}

void Config::setSku(const String& v) {
    _sku = v;
    Preferences p;
    p.begin(NS, false);
    p.putString(KEY_SKU, v);
    p.end();
}

void Config::printMasked() const {
    String masked;
    if (_apiKey.length() <= 8) {
        masked = _apiKey.length() ? "********" : "(unset)";
    } else {
        masked = _apiKey.substring(0, 4) + "..." + _apiKey.substring(_apiKey.length() - 4);
    }
    Serial.printf("Config:\n  apiKey: %s\n  device: %s\n  sku: %s\n",
                  masked.c_str(), _device.c_str(), _sku.c_str());
}
