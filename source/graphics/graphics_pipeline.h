//
// Created by Osprey on 6/4/2025.
//

#pragma once

#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <map>

#include "graphics_objects.h"
#include "glad/glad.h"
#include "../core/window.h"

#endif //GRAPHICS_PIPELINE_H

class GraphicsPipeline {
    Window* p_window;

    std::map<unsigned int, VertexArrayObject*> m_vaos;
    std::map<unsigned int, BufferObject<float>*> m_vbos;
    std::map<unsigned int, BufferObject<int>*> m_ibos;

    ShaderObject* m_unlitVertexShader;
    ShaderObject* m_unlitFragmentShader;
    ShaderProgramObject* m_unlitProgram;

public:
    GraphicsPipeline(Window* window);

    void RegisterModel(Model model);
    void RegisterScene(Scene scene);

    void Initialize();
    void RenderModel(Model model, Camera camera);
    void RenderScene(Scene scene);
    void PresentScene();

    void CleanUp();
};