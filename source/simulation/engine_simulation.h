//
// Created by Osprey on 6/17/2025.
//

#pragma once

#ifndef ENGINE_SIMULATION_H
#define ENGINE_SIMULATION_H
#include "../graphics/graphics_objects.h"

#endif //ENGINE_SIMULATION_H

struct ConnectableModel : Model {
    std::vector<glm::vec3> connectionPoints;
    float radius = 0.3f;

    ConnectableModel(Model model) : Model(model) {};

    int GetCurrentConnectionPointIndex(glm::vec2 mousePosition, glm::mat4 view, glm::mat4 projection, glm::ivec2 screenResolution);
};

struct Tank : ConnectableModel {
    Gas storedGas = {};
    float volume = 0.0f;
    float storedAmount = 0.0f;;

    Tank(Gas storedGas, float volume, float storedAmount);
};

struct Pump : ConnectableModel {
    Pump(Model model) : ConnectableModel(model) {};
};

struct ElectricPump : Pump {
    ElectricPump();
};

struct Engine : ConnectableModel {

};