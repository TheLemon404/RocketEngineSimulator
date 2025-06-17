//
// Created by Osprey on 6/17/2025.
//

#include "engine_simulation.h"

Tank::Tank(Gas storedGas, float volume, float storedAmount) : Model(Model::LoadModelFromOBJ("resources/meshes/tank.obj")), storedGas(storedGas), volume(volume), storedAmount(storedAmount) {
    meshes[0].material.color = storedGas.color;

    connectionPoints.emplace_back(0, -1.95,0);
}

ElectricPump::ElectricPump() : Pump(Model::LoadModelFromOBJ("resources/meshes/pump.obj")) {
    meshes[1].material.color = glm::vec3(0.8, 0.6, 0.4);
    connectionPoints.emplace_back(0,1.25,0);
    connectionPoints.emplace_back(0,0,-1.15);
}
