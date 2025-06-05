//
// Created by Osprey on 6/4/2025.
//

#include "graphics_pipeline.h"

#include "glm/ext/matrix_clip_space.hpp"

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
    glViewport(0,0,p_window->GetWindowDimentions().x,p_window->GetWindowDimentions().y);

    //load and compile shaders
    m_unlitVertexShader = new ShaderObject(GL_VERTEX_SHADER);
    m_unlitVertexShader->Load("resources/shaders/unlit.vert");
    m_unlitFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER);
    m_unlitFragmentShader->Load("resources/shaders/unlit.frag");
    m_unlitProgram = new ShaderProgramObject();
    m_unlitProgram->Compile(m_unlitVertexShader, m_unlitFragmentShader);
}

void GraphicsPipeline::RegisterScene(Scene scene) {
    for (Model model : scene.models) {
        RegisterModel(model);
    }
}

void GraphicsPipeline::RenderModel(Model model, Camera camera) {
    //draw
    m_unlitProgram->Use();

    //calculate matricies
    glm::mat4 transform;
    transform = glm::identity<glm::mat4>();

    glm::mat4 view;
    view = glm::lookAt(camera.position, camera.target, camera.up);

    glm::mat4 projection;
    if (camera.projectionMode == PERSPECTIVE) {
        projection = glm::perspective(glm::radians(camera.fov), (float)(p_window->GetWindowDimentions().x / p_window->GetWindowDimentions().y), 0.001f, 10000.0f);
    }
    else if (camera.projectionMode == ORTHOGRAPHIC) {
        float orthoWidth = ((float)p_window->GetWindowDimentions().x / 1000) * camera.zoomFactor;
        float orthoHeight = ((float)p_window->GetWindowDimentions().y / 1000) * camera.zoomFactor;
        projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0.001f, 10000.0f);
    }
    m_unlitProgram->UploadUniformMat4("projection", projection);
    m_unlitProgram->UploadUniformMat4("view", view);
    m_unlitProgram->UploadUniformMat4("transform", transform);

    m_vaos[model.id]->Bind();
    glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
    m_vaos[model.id]->Unbind();
}

void GraphicsPipeline::RenderScene(Scene scene) {
    //set opengl viewport and clear state
    glViewport(0,0,p_window->GetWindowDimentions().x,p_window->GetWindowDimentions().y);
    glClearColor(0.5, 0.8, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (Model model : scene.models) {
        RenderModel(model, scene.camera);
    }
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

