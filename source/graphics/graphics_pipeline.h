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

    //shaders
    ShaderObject* m_unlitVertexShader;
    ShaderObject* m_unlitFragmentShader;
    ShaderProgramObject* m_unlitProgram;

    ShaderObject* m_normalVertexShader;
    ShaderObject* m_normalFragmentShader;
    ShaderProgramObject* m_normalProgram;

    ShaderObject* m_gridVertexShader;
    ShaderObject* m_gridFragmentShader;
    ShaderProgramObject* m_gridProgram;

    ShaderObject* m_screenSpaceVertexShader;
    ShaderObject* m_checkersFragmentShader;
    ShaderProgramObject* m_checkersProgram;

    //fullscreen quad
    VertexArrayObject* m_quadVAO;
    BufferObject<float>* m_quadPositions;
    BufferObject<int>* m_quadIndices;

public:
    GraphicsPipeline(Window* window);

    void RegisterModel(Mesh& model);
    void RegisterScene(Scene& scene);

    void Initialize();
    void RenderModel(Mesh model, Camera camera);
    void RenderScene(Scene scene);
    void PresentScene();

    void CleanUp();
};