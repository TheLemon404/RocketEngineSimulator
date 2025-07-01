//
// Created by Osprey on 6/3/2025.
//

#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"
#include "../graphics/graphics_pipeline.h"
#include "../simulation/engine_simulation.h"

#endif //APPLICATION_H

class Application {
    std::string m_version;
    Window* m_window;
    GraphicsPipeline* m_graphicsPipeline;
    SimulationPipeline* m_simulationPipeline;
    Scene* m_scene;

    void MoveCamera();

public:
    Application(std::string version);

    void Initialize();
    void Run();
    void Close();

    ~Application();
};