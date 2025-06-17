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
};

struct Pump : Model {
    Pump(Model model) : Model(model) {};
};

struct ElectricPump : Pump {
    ElectricPump();
};

struct Engine : Model {

};