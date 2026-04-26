#pragma once

#include <Arduino.h>

struct SceneDef {
    const char* name;
    uint32_t paramId;
    uint32_t id;
};

// Add new scenes by appending one line. Names must be unique per HomeKit accessory.
constexpr SceneDef SCENES[] = {
    {"Afternoon Tea", 36951, 26278},
    {"Cyberpunk",  36967, 26293},
    {"Sunset",     1335,  1260},
    {"Aurora",     17876, 10671},
    {"Warm Sun",   36942, 26269},
    {"Meditation", 27446, 17291},
    {"Sleep",      36952, 26279},
    {"Movie",      1317,  1242},
};

constexpr size_t SCENE_COUNT = sizeof(SCENES) / sizeof(SCENES[0]);
