#pragma once

#include <HomeSpan.h>

class GoveeClient;
class SceneController;

class GoveeScene : public Service::Switch {
public:
    GoveeScene(const char* name, uint32_t paramId, uint32_t id,
               GoveeClient& client, SceneController& controller);

    boolean update() override;

    // Programmatically clear this switch (used by SceneController).
    // Sets the On characteristic to false and notifies HomeKit. Does NOT call the Govee API.
    void clear();

    const char* name() const { return _name; }

private:
    const char* _name;
    uint32_t _paramId;
    uint32_t _id;
    GoveeClient& _client;
    SceneController& _controller;
    SpanCharacteristic* _on;
};
