//
// Created by Osprey on 6/4/2025.
//

#pragma once

#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include "glad/glad.h"
#include "../core/window.h"

#endif //GRAPHICS_PIPELINE_H

class GraphicsPipeline {
    Window* p_window;

public:
    GraphicsPipeline(Window* window);
    void Initialize();
    void RenderScene();
    void PresentScene();
};