//
// Created by Osprey on 6/4/2025.
//

#include "graphics_pipeline.h"

GraphicsPipeline::GraphicsPipeline(Window *window) {
    p_window = window;
}

void GraphicsPipeline::Initialize() {
    if (!gladLoadGL()) {
        throw std::runtime_error("failed to initialize OpenGL");
    }

    glEnable(GL_CULL_FACE | GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0,0,p_window->GetDimentions().x,p_window->GetDimentions().y);
}

void GraphicsPipeline::RenderScene() {
    glViewport(0,0,p_window->GetDimentions().x,p_window->GetDimentions().y);
    glClearColor(0.5, 0.8, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsPipeline::PresentScene() {
    glfwSwapBuffers(p_window->GetGLFWWindow());
}
