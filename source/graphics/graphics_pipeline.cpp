//
// Created by Osprey on 6/4/2025.
//

#include "graphics_pipeline.h"

#include "glm/ext/matrix_clip_space.hpp"

GraphicsPipeline::GraphicsPipeline(Window *window) {
    p_window = window;
}

void GraphicsPipeline::RegisterModel(Mesh& model) {
    model.vao = new VertexArrayObject();
    model.vao->Bind();

    //vertex positions
    model.positionsBuffer = new BufferObject<float>();
    model.positionsBuffer->Upload(model.vertices);
    model.vao->CreateVertexAttributePointer(0, 3, sizeof(float), GL_FLOAT);

    //vertex normals
    if (model.normals.size() > 0) {
        model.normalsBuffer = new BufferObject<float>();
        model.normalsBuffer->Upload(model.normals);
        model.vao->CreateVertexAttributePointer(1, 3, sizeof(float), GL_FLOAT);
    }

    //vertex uvs
    if (model.uvs.size() > 0) {
        model.uvsBuffer = new BufferObject<float>();
        model.uvsBuffer->Upload(model.uvs);
        model.vao->CreateVertexAttributePointer(2, 2, sizeof(float), GL_FLOAT);
    }

    //indices
    model.indicesBuffer = new BufferObject<int>();
    model.indicesBuffer->Upload(model.indices);

    model.vao->Unbind();

    std::cout << "model: " << model.id << " has been registered" << std::endl;
}

void GraphicsPipeline::Initialize() {
    if (!gladLoadGL()) {
        throw std::runtime_error("failed to initialize OpenGL");
    }

    //initialize opengl settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    m_normalVertexShader = new ShaderObject(GL_VERTEX_SHADER);
    m_normalVertexShader->Load("resources/shaders/normal.vert");
    m_normalFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER);
    m_normalFragmentShader->Load("resources/shaders/normal.frag");
    m_normalProgram = new ShaderProgramObject();
    m_normalProgram->Compile(m_normalVertexShader, m_normalFragmentShader);

    m_gridVertexShader = new ShaderObject(GL_VERTEX_SHADER);
    m_gridVertexShader->Load("resources/shaders/grid.vert");
    m_gridFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER);
    m_gridFragmentShader->Load("resources/shaders/grid.frag");
    m_gridProgram = new ShaderProgramObject();
    m_gridProgram->Compile(m_gridVertexShader, m_gridFragmentShader);

    m_screenSpaceVertexShader = new ShaderObject(GL_VERTEX_SHADER);
    m_screenSpaceVertexShader->Load("resources/shaders/screenSpace.vert");
    m_checkersFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER);
    m_checkersFragmentShader->Load("resources/shaders/checkers.frag");
    m_checkersProgram = new ShaderProgramObject();
    m_checkersProgram->Compile(m_screenSpaceVertexShader, m_checkersFragmentShader);

    //create fullscreen quad
    m_quadVAO = new VertexArrayObject();
    m_quadVAO->Bind();

    m_quadPositions = new BufferObject<float>();
    m_quadPositions->Upload({
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0, 1.0, 0.0f
    });
    m_quadVAO->CreateVertexAttributePointer(0, 3, sizeof(float), GL_FLOAT);

    m_quadIndices = new BufferObject<int>();
    m_quadIndices->Upload({0, 1, 2, 2, 3, 0});
}

void GraphicsPipeline::RegisterScene(Scene& scene) {
    for (int i = 0; i < scene.models.size(); i++) {
        RegisterModel(scene.models[i]);
    }
}

void GraphicsPipeline::RenderModel(Mesh model, Camera camera) {
    //draw
    switch (model.renderMode) {
        case NORMAL:
            m_normalProgram->Use();
            break;
        case UNLIT:
            m_unlitProgram->Use();
            break;
        default:
            break;
    }

    //calculate matricies
    glm::mat4 transform;
    transform = glm::identity<glm::mat4>();

    glm::mat4 view;
    view = glm::lookAt(camera.position, camera.target, camera.up);

    glm::mat4 projection;
    if (camera.projectionMode == PERSPECTIVE) {
        projection = glm::perspective(glm::radians(camera.fov), ((float)p_window->GetWindowDimentions().x / (float)p_window->GetWindowDimentions().y), 0.001f, 10000.0f);
    }
    else if (camera.projectionMode == ORTHOGRAPHIC) {
        float orthoWidth = ((float)p_window->GetWindowDimentions().x / 1000) * camera.zoomFactor;
        float orthoHeight = ((float)p_window->GetWindowDimentions().y / 1000) * camera.zoomFactor;
        projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0.001f, 10000.0f);
    }
    m_unlitProgram->UploadUniformMat4("projection", projection);
    m_unlitProgram->UploadUniformMat4("view", view);
    m_unlitProgram->UploadUniformMat4("transform", transform);

    model.vao->Bind();
    glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
    model.vao->Unbind();

    //stop using any shader program
    glUseProgram(0);
}

void GraphicsPipeline::RenderScene(Scene scene) {
    //set opengl viewport and clear state
    glViewport(0,0,p_window->GetWindowDimentions().x,p_window->GetWindowDimentions().y);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    //render checkered background
    m_checkersProgram->Use();
    m_checkersProgram->UploadUniformVec2("resolution",p_window->GetWindowDimentions());
    m_quadVAO->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_quadVAO->Unbind();
    glUseProgram(0);

    //render infinite grid
    m_gridProgram->Use();
    glm::mat4 view = glm::lookAt(scene.camera.position, scene.camera.target, scene.camera.up);
    glm::mat4 projection;
    if (scene.camera.projectionMode == PERSPECTIVE) {
        projection = glm::perspective(glm::radians(scene.camera.fov), ((float)p_window->GetWindowDimentions().x / (float)p_window->GetWindowDimentions().y), 0.001f, 10000.0f);
    }
    else if (scene.camera.projectionMode == ORTHOGRAPHIC) {
        float orthoWidth = ((float)p_window->GetWindowDimentions().x / 1000) * scene.camera.zoomFactor;
        float orthoHeight = ((float)p_window->GetWindowDimentions().y / 1000) * scene.camera.zoomFactor;
        projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0.001f, 10000.0f);
    }
    m_gridProgram->UploadUniformMat4("view", view);
    m_gridProgram->UploadUniformMat4("projection", projection);
    m_quadVAO->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_quadVAO->Unbind();
    glUseProgram(0);

    glEnable(GL_DEPTH_TEST);

    //render models in scene
    for (int i = 0; i < scene.models.size(); i++) {
        RenderModel(scene.models[i], scene.camera);
    }
}


void GraphicsPipeline::PresentScene() {
    glfwSwapBuffers(p_window->GetGLFWWindow());
}

void GraphicsPipeline::CleanUp() {
    delete m_unlitProgram;
}

