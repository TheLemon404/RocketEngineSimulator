//
// Created by Osprey on 6/17/2025.
//

#pragma once

#ifndef ENGINE_SIMULATION_H
#define ENGINE_SIMULATION_H
#include "../graphics/graphics_objects.h"

#endif //ENGINE_SIMULATION_H

struct Tank : Model {
    Gas storedGas = {};
    float volume = 0.0f;
    float storedAmount = 0.0f;;

    Tank(Gas storedGas, float volume, float storedAmount);

    void ComputeControlPointPressure();
};

struct Pump : Model {
    explicit Pump(const Model& model) : Model(model) {};
};

struct ElectricPump : Pump {
    ElectricPump();

    void ComputeControlPointPressure();
};

struct Engine : Model {
    Engine();

    void ComputeControlPointPressure();
};

class SimulationPipeline {
private:

public:
    void Initialize();
    void RegisterScene(Scene* scene);
    void StepSimulation(Scene* scene);
};