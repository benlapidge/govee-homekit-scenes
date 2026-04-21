#include "SceneController.h"

#include "GoveeScene.h"

void SceneController::clearAllExcept(GoveeScene* keep) {
    for (auto* s : _scenes) {
        if (s != keep) s->clear();
    }
}

void SceneController::clearAll() {
    for (auto* s : _scenes) s->clear();
}
