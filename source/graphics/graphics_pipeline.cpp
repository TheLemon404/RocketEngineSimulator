//
// Created by Osprey on 6/4/2025.
//

#include "graphics_pipeline.h"

GraphicsPipeline::GraphicsPipeline(Window *window) {
    p_window = window;
}

void GraphicsPipeline::RegisterModel(Model model) {
    m_vaos[model.id] = new VertexArrayObject();
    m_vaos[model.id]->Bind();

    //vertex positions
    m_vbos[model.id] = new BufferObject<float>();
    m_vbos[model.id]->Upload(model.vertices);
    m_vaos[model.id]->CreateVertexAttributePointer(0, 3, sizeof(float), GL_FLOAT);

    //indices
    m_ibos[model.id] = new BufferObject<int>();
    m_ibos[model.id]->Upload(model.indices);

    m_vaos[model.id]->Unbind();

    std::cout << "model: " << model.id << " has been registered" << std::endl;
}

void GraphicsPipeline::Initialize() {
    if (!gladLoadGL()) {
        throw std::runtime_error("failed to initialize OpenGL");
    }

    //initialize opengl settings
    glEnable(GL_CULL_FACE | GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0,0,p_window->GetDimentions().x,p_window->GetDimentions().y * 2.0f);

    //load and compile shaders
    m_unlitVertexShader = new ShaderObject(GL_VERTEX_SHADER);
    m_unlitVertexShader->Load("resources/shaders/unlit.vert");
    m_unlitFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER);
    m_unlitFragmentShader->Load("resources/shaders/unlit.frag");
    m_unlitProgram = new ShaderProgramObject();
    m_unlitProgram->Compile(m_unlitVertexShader, m_unlitFragmentShader);
}

void GraphicsPipeline::RenderScene(Model model) {
    //set opengl viewport and clear state
    glViewport(0,0,p_window->GetDimentions().x,p_window->GetDimentions().y * 2.0f);
    glClearColor(0.5, 0.8, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw
    m_unlitProgram->Use();

    m_vaos[model.id]->Bind();
    glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
    m_vaos[model.id]->Unbind();
}

void GraphicsPipeline::PresentScene() {
    glfwSwapBuffers(p_window->GetGLFWWindow());
}

void GraphicsPipeline::CleanUp() {
    for (const auto& pair : m_vaos) {
        delete pair.second;
    }

    for (const auto& pair : m_vbos) {
        delete pair.second;
    }

    for (const auto& pair : m_ibos) {
        delete pair.second;
    }

    delete m_unlitProgram;
}

