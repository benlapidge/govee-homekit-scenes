#include "GoveeClient.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "Config.h"

namespace {
constexpr const char* CONTROL_URL =
    "https://openapi.api.govee.com/router/api/v1/device/control";
constexpr uint16_t TIMEOUT_MS = 5000;
}

bool GoveeClient::setPower(bool on) {
    JsonDocument d;
    d.set(on ? 1 : 0);
    return postCapability("devices.capabilities.on_off", "powerSwitch", d.as<JsonVariantConst>());
}

bool GoveeClient::setBrightness(uint8_t pct) {
    if (pct < 1) pct = 1;
    if (pct > 100) pct = 100;
    JsonDocument d;
    d.set(pct);
    return postCapability("devices.capabilities.range", "brightness", d.as<JsonVariantConst>());
}

bool GoveeClient::setColorRgb(uint32_t rgb) {
    JsonDocument d;
    d.set(rgb & 0xFFFFFF);
    return postCapability("devices.capabilities.color_setting", "colorRgb", d.as<JsonVariantConst>());
}

bool GoveeClient::setColorTempK(uint16_t kelvin) {
    if (kelvin < 2200) kelvin = 2200;
    if (kelvin > 6500) kelvin = 6500;
    JsonDocument d;
    d.set(kelvin);
    return postCapability("devices.capabilities.color_setting", "colorTemperatureK", d.as<JsonVariantConst>());
}

bool GoveeClient::setScene(uint32_t paramId, uint32_t id) {
    JsonDocument d;
    d["paramId"] = paramId;
    d["id"] = id;
    return postCapability("devices.capabilities.dynamic_scene", "lightScene", d.as<JsonVariantConst>());
}

bool GoveeClient::postCapability(const char* type, const char* instance,
                                 JsonVariantConst value) {
    if (!_cfg.isConfigured()) {
        Serial.println("[GoveeClient] no api key set; skipping call");
        return false;
    }

    JsonDocument body;
    body["requestId"] = String(millis());
    JsonObject payload = body["payload"].to<JsonObject>();
    payload["sku"] = _cfg.sku();
    payload["device"] = _cfg.device();
    JsonObject cap = payload["capability"].to<JsonObject>();
    cap["type"] = type;
    cap["instance"] = instance;
    cap["value"] = value;

    String json;
    serializeJson(body, json);

    WiFiClientSecure secure;
    secure.setInsecure();  // TODO: pin Govee root CA
    HTTPClient http;
    http.setTimeout(TIMEOUT_MS);
    if (!http.begin(secure, CONTROL_URL)) {
        Serial.println("[GoveeClient] http.begin failed");
        return false;
    }
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Govee-API-Key", _cfg.apiKey());

    Serial.printf("[GoveeClient] POST %s/%s -> %s\n", type, instance, json.c_str());
    int code = http.POST(json);
    bool ok = code == 200;
    if (!ok) {
        String resp = http.getString();
        Serial.printf("[GoveeClient] HTTP %d: %s\n", code, resp.c_str());
    } else {
        Serial.printf("[GoveeClient] HTTP 200\n");
    }
    http.end();
    return ok;
}
