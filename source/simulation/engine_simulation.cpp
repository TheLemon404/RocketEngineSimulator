//
// Created by Osprey on 6/17/2025.
//

#include "engine_simulation.h"

#include <iostream>
#include <memory>

Tank::Tank(Gas storedGas, float volume, float storedAmount) : Model(Model::LoadModelFromOBJ("resources/meshes/tank.obj")), storedGas(storedGas), volume(volume), storedAmount(storedAmount) {
    meshes[0].material.color = storedGas.color;

    connectionPoints.emplace_back(0, -1.95,0);
}

void Tank::ComputeControlPointPressure() {
    for (int i = 0; i < connectedControls.size(); i++) {
        Control* c = connectedControls[i];
        c->controlPointPressure = 10.0f;
    }
}

ElectricPump::ElectricPump() : Pump(Model::LoadModelFromOBJ("resources/meshes/pump.obj")) {
    meshes[1].material.color = glm::vec3(0.8, 0.6, 0.4);
    connectionPoints.emplace_back(0,1.25,0);
    connectionPoints.emplace_back(0,0,-1.15);
}

void ElectricPump::ComputeControlPointPressure() {
    for (int i = 0; i < connectedControls.size(); i++) {
        Control* c = connectedControls[i];

        if (c->position == connectionPoints[0]) {
            c->controlPointPressure = -5.0f;
        }
        else if (c->position == connectionPoints[1]) {
            c->controlPointPressure = 5.0f;
        }
    }
}

Engine::Engine() : Model(Model::LoadModelFromOBJ("resources/meshes/test_engine.obj")){

}

void Engine::ComputeControlPointPressure() {
    for (int i = 0; i < connectedControls.size(); i++) {
        Control* c = connectedControls[i];
        c->controlPointPressure = -10.0f;
    }
}

void SimulationPipeline::Initialize() {

}

void SimulationPipeline::RegisterScene(Scene* scene) {

}

void SimulationPipeline::StepSimulation(Scene* scene) {
    //first allow for simulation objects to input and remove pressure from pipes
    for ( int i = 0; i < scene->models.size(); i++) {
        if (dynamic_cast<Tank*>(scene->models[i])){
            Tank* tank = dynamic_cast<Tank*>(scene->models[i]);
            tank->ComputeControlPointPressure();
        }
        else if (dynamic_cast<Pump*>(scene->models[i])) {
            Pump* pump = dynamic_cast<Pump*>(scene->models[i]);
            if (dynamic_cast<ElectricPump*>(pump)) {
                ElectricPump* electricPump = dynamic_cast<ElectricPump*>(pump);
                electricPump->ComputeControlPointPressure();
            }
        }
        else if (dynamic_cast<Engine*>(scene->models[i])) {
            Engine* engine = dynamic_cast<Engine*>(scene->models[i]);
            engine->ComputeControlPointPressure();
        }
    }

    //now compute the final pressure of the pipes
    for (int i = 0; i < scene->pipes.size(); i++) {
        Pipe* p = scene->pipes[i];
        p->totalInternalPressure = (p->path.controls[0].controlPointPressure + p->path.controls[1].controlPointPressure) / 2.0f;
    }
}
