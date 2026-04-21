#include "GoveeScene.h"

#include "GoveeClient.h"
#include "SceneController.h"

GoveeScene::GoveeScene(const char* name, uint32_t paramId, uint32_t id,
                       GoveeClient& client, SceneController& controller)
    : Service::Switch(),
      _name(name), _paramId(paramId), _id(id),
      _client(client), _controller(controller) {
    _on = new Characteristic::On(false);
    new Characteristic::Name(name);
    new Characteristic::ConfiguredName(name);
}

boolean GoveeScene::update() {
    bool turningOn = _on->getNewVal<int>() != 0;
    if (turningOn) {
        Serial.printf("[Scene] activating %s\n", _name);
        _client.setScene(_paramId, _id);
        _controller.clearAllExcept(this);
    }
    // Turning off has no API call: Govee has no "stop scene".
    return true;
}

void GoveeScene::clear() {
    if (_on->getVal<int>() != 0) {
        _on->setVal(0);
    }
}
