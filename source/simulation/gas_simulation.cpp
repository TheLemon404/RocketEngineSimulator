//
// Created by Osprey on 6/13/2025.
//

#include "gas_simulation.h"

#include <cmath>
#include <iostream>

GasSimulation::GasSimulation(int resolution) : resolution(resolution)
{
    for (int i = 0; i < resolution; i++) {
        regions.emplace_back((float)1/(float)resolution);
    }

    for (int i = 0; i < resolution - 1; i++) {
        fluxes.emplace_back(0, 0, 0);
    }

    //establish initial conservative values
    for (int i = 0; i < regions.size(); i++) {
        if (i < regions.size() / 2) {
            regions[i].density = 1.0f;
            regions[i].velocity = 0.0f;
            regions[i].pressure = 1.0f;
        }
        else {
            regions[i].density = 0.125f;
            regions[i].velocity = 0.0f;
            regions[i].pressure = 0.1f;
        }

        regions[i].energy = regions[i].pressure / (gamma - 1.0f) + 0.5f * regions[i].density * pow(regions[i].velocity, 2);
        regions[i].conservatives = {
            regions[i].density,
            regions[i].density * regions[i].velocity,
            regions[i].energy
        };
    }
}

glm::vec3 GasSimulation::RiemannSolver(glm::vec3 ul, glm::vec3 ur) {
    float rhoL = ul.x;
    float uL = ul.y / rhoL;
    float EL = ul.z;
    float rhoR = ur.x;
    float uR = ur.y / rhoR;
    float ER = ur.z;

    float pL = (gamma - 1.0f) * (EL - 0.5f * rhoL * pow(uL, 2));
    float pR = (gamma - 1.0f) * (ER - 0.5f * rhoR * pow(uR, 2));

    float cL = sqrt(gamma * pL / rhoL);
    float cR = sqrt(gamma * pR / rhoR);

    glm::vec3 FL = {rhoL * uL, rhoL * pow(uL, 2), uL * (EL + pL)};
    glm::vec3 FR = {rhoR * uR, rhoR * pow(uR, 2), uR * (ER + pR)};

    float SL = std::min(uL - cL, uR - cR);
    float SR = std::max(uL + cL, uR + cR);

    if (SL >= 0) {
        return FL;
    }
    else if (SR <= 0) {
        return FR;
    }
    else {
        return (SR * FL - SL * FR + SL * SR * (ur - ul)) / (SR - SL);
    }
}

void GasSimulation::ComputeState() {
    //compute interface fluxes
    for (int i = 0; i < regions.size() - 1; i++) {
        glm::vec3 ul = regions[i].conservatives;
        glm::vec3 ur = regions[i + 1].conservatives;
        fluxes[i] = RiemannSolver(ul, ur);
    }

    //compute delta time
    float maxSpeed = 0;
    for (int i = 0; i < regions.size(); i++) {
        float c = sqrt(gamma * regions[i].pressure / regions[i].density);
        maxSpeed = std::max(maxSpeed, abs(regions[i].velocity) + c);
    }
    float dt = CFL * regions[0].size / maxSpeed; // assuming uniform grid

    //evaluate
    std::vector<glm::vec3> updated;
    updated.resize(regions.size());
    updated[0] = regions[1].conservatives;
    updated[0].y = -updated[0].y; // flip momentum
    updated[regions.size() - 1] = regions[regions.size() - 2].conservatives;
    updated[regions.size() - 1].y = -updated[regions.size() - 1].y;
    for (int i = 1; i < regions.size() - 1; i++) {
        updated[i] = regions[i].conservatives - (dt / regions[i].size) * (fluxes[i] - fluxes[i - 1]);
    }

    for (int i = 0; i < regions.size(); i++) {
        regions[i].conservatives = updated[i];
    }
}

void GasSimulation::UpdatePrimatives() {
    for (auto& region : regions) {
        float rho = region.conservatives.x;
        float momentum = region.conservatives.y;
        float energy = region.conservatives.z;

        region.density = rho;
        region.velocity = momentum / rho;
        region.energy = energy;
        region.pressure = (gamma - 1.0f) * (energy - 0.5f * rho * region.velocity * region.velocity);
    }
}

void GasSimulation::Step() {
    ComputeState();
    UpdatePrimatives();
}
