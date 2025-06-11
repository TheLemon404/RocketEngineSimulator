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

void DebugLinks(Scene& scene);

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

    ShaderObject* m_linePathVertexShader;
    ShaderObject* m_linePathFragmentShader;
    ShaderObject* m_linePathTesselationEvaluationShader;
    ShaderObject* m_linePathTesselationControlShader;
    ShaderProgramObject* m_linePathProgram;

    //fullscreen quad
    VertexArrayObject* m_quadVAO;
    BufferObject<float>* m_quadPositions;
    BufferObject<int>* m_quadIndices;

    //for selecting gizmos
    bool m_isLineSelected = false;
    int m_currentSelectedControlIndex = -1;
    int m_currentSelectedLinePathIndex = -1;
    int m_currentSelectedPipeIndex = -1;


public:
    GraphicsPipeline(Window* window);

    void RegisterMesh(Mesh& mesh);
    void RegisterLinePath(LinePath& linePath);
    void RegisterPipe(Pipe& pipe);
    void RegisterScene(Scene& scene);

    void Initialize();

    //selection
    void ClearSelection(Scene& scene);

    //immediate-mode drawing
    void DrawDebugSphere3D(glm::vec3 center, float radius, glm::vec3 color, Camera camera);
    void DrawDebugCircle2D(glm::vec2 center, float radius, glm::vec3 color);
    void DrawLinePathGizmos(LinePath linePath, Camera camera);

    //non rendering per-frame operations
    void UpdateGeometry(Scene& scene);

    //rendering
    void RenderMesh(Mesh& mesh, glm::mat4 view, glm::mat4 projection);
    void RenderLinePath(LinePath& linePath, glm::mat4 view, glm::mat4 projection);
    void RenderPipe(Pipe& pipe, glm::mat4 view, glm::mat4 projection);
    void RenderScene(Scene& scene);
    void DrawUI(Scene& scene);

    void CleanUp();
};