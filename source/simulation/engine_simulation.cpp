//
// Created by Osprey on 6/17/2025.
//

#include "engine_simulation.h"

#include <iostream>
#include <memory>

Tank::Tank(Gas storedGas, float volume, float storedAmount) : Model(Model::LoadModelFromOBJ("resources/meshes/tank.obj")), storedGas(storedGas), volume(volume), storedAmount(storedAmount) {
    meshes[0].material.color = storedGas.color;

    simulationObjectType = TANK;

    connectionPoints.emplace_back(0, -1.95,0);
}

ElectricPump::ElectricPump() : Pump(Model::LoadModelFromOBJ("resources/meshes/pump.obj")) {
    meshes[1].material.color = glm::vec3(0.8, 0.6, 0.4);
    connectionPoints.emplace_back(0,1.25,0);
    connectionPoints.emplace_back(0,0,-1.15);

    simulationObjectType = PUMP;
}

Engine::Engine() : Model(Model::LoadModelFromOBJ("resources/meshes/test_engine.obj")){
    simulationObjectType = ENGINE;
}

void SimulationPipeline::Initialize() {

}

void SimulationPipeline::RegisterScene(Scene* scene) {

}

void SimulationPipeline::StepSimulation(Scene* scene) {
    for (int i = 0; i < scene->pipes.size(); i++) {
        Pipe* pipe = scene->pipes[i];
        Control& p1 = scene->pipes[i]->path.controls[0];
        Control& p2 = scene->pipes[i]->path.controls[scene->pipes[i]->path.controls.size()-1];

        //first calculate incoming pressure from tanks
        if (p1.connectedSimulationObjectType == SimulationObjectType::TANK) {
            Tank* tank = static_cast<Tank*>(p1.connectedSimulationObjectModel);
            p1.controlPointPressure = tank->storedAmount;
        }
        if (p2.connectedSimulationObjectType == SimulationObjectType::TANK) {
            Tank* tank = static_cast<Tank*>(p2.connectedSimulationObjectModel);
            p2.controlPointPressure = tank->storedAmount;
        }

        //account for pressure from pumps
        if (p1.connectedSimulationObjectType == SimulationObjectType::PUMP) {
            Pump* pump = static_cast<Pump*>(p1.connectedSimulationObjectModel);
            p1.controlPointPressure = pump->pressureMultiplier;
        }
        if (p2.connectedSimulationObjectType == SimulationObjectType::PUMP) {
            Pump* pump = static_cast<Pump*>(p2.connectedSimulationObjectModel);
            p2.controlPointPressure = pump->pressureMultiplier;
        }

        //account for pressure impacted by engines
        if (p1.connectedSimulationObjectType == SimulationObjectType::ENGINE) {
            Engine* engine = static_cast<Engine*>(p1.connectedSimulationObjectModel);
        }
        if (p2.connectedSimulationObjectType == SimulationObjectType::ENGINE) {
            Engine* engine = static_cast<Engine*>(p2.connectedSimulationObjectModel);
        }

        pipe->totalInternalPressure = (p1.controlPointPressure + p2.controlPointPressure) / 2.0f;
    }
}
