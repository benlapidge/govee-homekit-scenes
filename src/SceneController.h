#pragma once

#include <vector>

class GoveeScene;

class SceneController {
public:
    void add(GoveeScene* s) { _scenes.push_back(s); }

    // Clear every scene switch except `keep` (typically the one just activated).
    void clearAllExcept(GoveeScene* keep);

    // Clear every scene switch. Used when the lamp is controlled directly.
    void clearAll();

private:
    std::vector<GoveeScene*> _scenes;
};
