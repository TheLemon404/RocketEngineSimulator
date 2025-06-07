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

    ShaderObject* m_splineVertexShader;
    ShaderObject* m_splineFragmentShader;
    ShaderObject* m_splineTesselationEvaluationShader;
    ShaderObject* m_splineTesselationControlShader;
    ShaderProgramObject* m_splineProgram;

    //fullscreen quad
    VertexArrayObject* m_quadVAO;
    BufferObject<float>* m_quadPositions;
    BufferObject<int>* m_quadIndices;

public:
    GraphicsPipeline(Window* window);

    void RegisterMesh(Mesh& mesh);
    void RegisterSpline(Spline& spline);
    void RegisterScene(Scene& scene);

    void Initialize();
    void Rendermesh(Mesh mesh, Camera camera, glm::mat4 view, glm::mat4 projection);
    void RenderSpline(Spline spline, Camera camera, glm::mat4 view, glm::mat4 projection);
    void RenderScene(Scene scene);
    void PresentScene();

    void CleanUp();
};