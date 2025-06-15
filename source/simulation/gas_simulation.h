//
// Created by Osprey on 6/13/2025.
//

#pragma once

#ifndef GAS_SIMULATION_H
#define GAS_SIMULATION_H
#include <vector>

#include "glm/vec3.hpp"

#endif //GAS_SIMULATION_H

struct GasRegion {
    float size = 0;
    float density = 1.0f;
    float velocity = 0.0f;
    float pressure = 1.0f;
    float energy = 0.0f;
    glm::vec3 conservatives;
    glm::vec3 flux;
    glm::vec3 previousFluxHalf = glm::vec3(1.0f);
    GasRegion(float size) : size(size) {};
};

class GasSimulation {
    int resolution = 32;
;
    glm::vec3 RiemannSolver(glm::vec3 ul, glm::vec3 ur);
    void ComputeState();
    void UpdatePrimatives();

public:
    std::vector<GasRegion> regions;
    std::vector<glm::vec3> fluxes;
    float gamma = 1.4f;
    float CFL = 0.9f;

    GasSimulation(int resolution = 32);

    void SetState(int regionIndex, float density, float velocity, float pressure);
    void Step();
};