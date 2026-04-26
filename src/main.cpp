#include <Arduino.h>
#include <HomeSpan.h>

#include "Config.h"
#include "GoveeClient.h"
#include "GoveeLight.h"
#include "GoveeScene.h"
#include "SceneController.h"
#include "SceneRegistry.h"

namespace {
Config gConfig;
GoveeClient gClient(gConfig);
SceneController gController;

// HomeSpan's SpanUserCommand callback hands us the buffer starting AT the
// command character (e.g. "K abc123"), not after it — skip the first char,
// then strip surrounding whitespace.
String extractArg(const char* s) {
    if (*s) ++s;
    String v(s);
    v.trim();
    return v;
}

void cmdSetKey(const char* buf) {
    String v = extractArg(buf);
    if (v.length() == 0) {
        Serial.println("usage: @K <api-key>");
        return;
    }
    gConfig.setApiKey(v);
    Serial.println("[cmd] api key updated");
    gConfig.printMasked();
}

void cmdSetDevice(const char* buf) {
    String v = extractArg(buf);
    if (v.length() == 0) {
        Serial.println("usage: @D <device-id>");
        return;
    }
    gConfig.setDevice(v);
    Serial.println("[cmd] device updated");
    gConfig.printMasked();
}

void cmdSetSku(const char* buf) {
    String v = extractArg(buf);
    if (v.length() == 0) {
        Serial.println("usage: @S <sku>");
        return;
    }
    gConfig.setSku(v);
    Serial.println("[cmd] sku updated");
    gConfig.printMasked();
}

void cmdPrintConfig(const char* /*buf*/) {
    gConfig.printMasked();
}
}  // namespace

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n[boot] Govee HomeKit Bridge");

    gConfig.load();
    gConfig.printMasked();
    if (!gConfig.isConfigured()) {
        Serial.println("[boot] no api key configured. set with: @K <api-key>");
    }

    homeSpan.setLogLevel(1);
    homeSpan.setStatusPin(2);
    homeSpan.setApSSID("Govee-Bridge-Setup");

    new SpanUserCommand('K', " <key> - set Govee API key", cmdSetKey);
    new SpanUserCommand('D', " <id>  - set Govee device id", cmdSetDevice);
    new SpanUserCommand('S', " <sku> - set Govee SKU", cmdSetSku);
    new SpanUserCommand('P', "       - print current config", cmdPrintConfig);

    homeSpan.begin(Category::Lighting, "Govee Floor Lamp");

    new SpanAccessory();
    new Service::AccessoryInformation();
    new Characteristic::Identify();
    new Characteristic::Manufacturer("Govee (via HomeSpan)");
    new Characteristic::Model("H6076");
    new Characteristic::Name("Govee Floor Lamp");
    new Characteristic::SerialNumber("govee-h6076-1");
    new Characteristic::FirmwareRevision("0.1.0");

    new GoveeLight(gClient, [] { gController.clearAll(); });

    for (size_t i = 0; i < SCENE_COUNT; ++i) {
        const auto& s = SCENES[i];
        gController.add(new GoveeScene(s.name, s.paramId, s.id, gClient, gController));
    }
}

void loop() {
    homeSpan.poll();
}
