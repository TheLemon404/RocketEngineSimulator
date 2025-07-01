//
// Created by Osprey on 6/17/2025.
//

#pragma once

#ifndef ENGINE_SIMULATION_H
#define ENGINE_SIMULATION_H
#include "../graphics/graphics_objects.h"

#endif //ENGINE_SIMULATION_H

enum SimulationObjectType {
    NONE,
    TANK,
    PUMP,
    ENGINE
};

struct Tank : Model {
    Gas storedGas = {};
    float volume = 0.0f;
    float storedAmount = 0.0f;;

    Tank(Gas storedGas, float volume, float storedAmount);
};

struct Pump : Model {
    Pump(Model model) : Model(model) {};

    float pressureMultiplier = 1.5f;
};

struct ElectricPump : Pump {
    ElectricPump();
};

struct Engine : Model {
    Engine();
};

class SimulationPipeline {
private:

public:
    void Initialize();
    void RegisterScene(Scene* scene);
    void StepSimulation(Scene* scene);
};