//
// Created by Osprey on 6/13/2025.
//

#pragma once

#ifndef GAS_SIMULATION_H
#define GAS_SIMULATION_H
#include <vector>

#include "../graphics/graphics_objects.h"

#endif //GAS_SIMULATION_H

struct GasRegion {
    float radius;
    float width;
};

class GasSimulation {
private:
    Pipe& m_pipe;

public:
    std::pmr::vector<GasRegion> regions;

    GasSimulation(Pipe& pipe) : m_pipe(pipe) {}
};