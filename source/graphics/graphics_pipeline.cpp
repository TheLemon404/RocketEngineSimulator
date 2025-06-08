//
// Created by Osprey on 6/4/2025.
//

#include "graphics_pipeline.h"

#include "glm/ext/matrix_clip_space.hpp"

GraphicsPipeline::GraphicsPipeline(Window *window) {
    p_window = window;
}

void GraphicsPipeline::RegisterMesh(Mesh& mesh) {
    mesh.vao = new VertexArrayObject();
    mesh.vao->Bind();

    //vertex positions
    mesh.positionsBuffer = new BufferObject<float>();
    mesh.positionsBuffer->Upload(mesh.vertices);
    mesh.vao->CreateVertexAttributePointer(0, 3, sizeof(float), GL_FLOAT);

    //vertex normals
    if (mesh.normals.size() > 0) {
        mesh.normalsBuffer = new BufferObject<float>();
        mesh.normalsBuffer->Upload(mesh.normals);
        mesh.vao->CreateVertexAttributePointer(1, 3, sizeof(float), GL_FLOAT);
    }

    //vertex uvs
    if (mesh.uvs.size() > 0) {
        mesh.uvsBuffer = new BufferObject<float>();
        mesh.uvsBuffer->Upload(mesh.uvs);
        mesh.vao->CreateVertexAttributePointer(2, 2, sizeof(float), GL_FLOAT);
    }

    //indices
    mesh.indicesBuffer = new BufferObject<int>();
    mesh.indicesBuffer->Upload(mesh.indices);

    mesh.vao->Unbind();

    std::cout << "mesh: " << mesh.id << " has been registered" << std::endl;
}

void GraphicsPipeline::RegisterSpline(Spline &spline) {
    spline.vao = new VertexArrayObject();
    spline.vao->Bind();

    //vertex positions
    spline.positionsBuffer = new BufferObject<float>();
    spline.positionsBuffer->Upload(spline.ExtractPositions());
    spline.vao->CreateVertexAttributePointer(0, 3, sizeof(float), GL_FLOAT);

    spline.vao->Unbind();

    std::cout << "spline: " << spline.id << " has been registered" << std::endl;
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
    glPatchParameteri(GL_PATCH_VERTICES, 4);

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

    m_splineVertexShader = new ShaderObject(GL_VERTEX_SHADER);
    m_splineVertexShader->Load("resources/shaders/spline.vert");
    m_splineFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER);
    m_splineFragmentShader->Load("resources/shaders/spline.frag");
    m_splineTesselationControlShader = new ShaderObject(GL_TESS_CONTROL_SHADER);
    m_splineTesselationControlShader->Load("resources/shaders/spline.tcs");
    m_splineTesselationEvaluationShader = new ShaderObject(GL_TESS_EVALUATION_SHADER);
    m_splineTesselationEvaluationShader->Load("resources/shaders/spline.tes");
    m_splineProgram = new ShaderProgramObject();
    m_splineProgram->CompileTesselation(m_splineVertexShader, m_splineTesselationControlShader, m_splineTesselationEvaluationShader, m_splineFragmentShader);

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
    for (int i = 0; i < scene.meshes.size(); i++) {
        RegisterMesh(scene.meshes[i]);
    }
    for (int i = 0; i < scene.splines.size(); i++) {
        RegisterSpline(scene.splines[i]);
    }
}

void GraphicsPipeline::DrawSplineGizmos(Spline spline) {

}

void GraphicsPipeline::Rendermesh(Mesh mesh, Camera camera, glm::mat4 view, glm::mat4 projection) {
    //draw
    switch (mesh.renderMode) {
        case NORMAL:
            m_normalProgram->Use();
            break;
        case UNLIT:
            m_unlitProgram->Use();
            break;
        default:
            break;
    }

    glm::mat4 transform = glm::identity<glm::mat4>();

    m_unlitProgram->UploadUniformMat4("projection", projection);
    m_unlitProgram->UploadUniformMat4("view", view);
    m_unlitProgram->UploadUniformMat4("transform", transform);

    mesh.vao->Bind();
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    mesh.vao->Unbind();

    //stop using any shader program
    glUseProgram(0);
}

void GraphicsPipeline::RenderSpline(Spline spline, Camera camera, glm::mat4 view, glm::mat4 projection) {
    //draw curves (for debug)
    m_splineProgram->Use();
    m_splineProgram->UploadUniformMat4("view", view);
    m_splineProgram->UploadUniformMat4("projection", projection);
    m_splineProgram->UploadUniformVec4("tint", glm::vec4(1.0f));
    m_splineProgram->UploadUniformMat4("transform", glm::identity<glm::mat4>());
    m_splineProgram->UploadUniformFloat("segmentCount", 40);
    m_splineProgram->UploadUniformFloat("stripCount", 1);
    spline.vao->Bind();
    glDrawArrays(GL_PATCHES, 0, 4);
    spline.vao->Unbind();
    glUseProgram(0);
}

void GraphicsPipeline::RenderScene(Scene scene) {
    //set opengl viewport and clear state
    glViewport(0,0,p_window->GetWindowDimentions().x,p_window->GetWindowDimentions().y);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //calculate matricies
    glm::mat4 view;
    view = glm::lookAt(scene.camera.position, scene.camera.target, scene.camera.up);

    glm::mat4 projection;
    if (scene.camera.projectionMode == PERSPECTIVE) {
        projection = glm::perspective(glm::radians(scene.camera.fov), ((float)p_window->GetWindowDimentions().x / (float)p_window->GetWindowDimentions().y), 0.001f, 10000.0f);
    }
    else if (scene.camera.projectionMode == ORTHOGRAPHIC) {
        float orthoWidth = ((float)p_window->GetWindowDimentions().x / 1000) * scene.camera.zoomFactor;
        float orthoHeight = ((float)p_window->GetWindowDimentions().y / 1000) * scene.camera.zoomFactor;
        projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0.001f, 10000.0f);
    }

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
    m_gridProgram->UploadUniformMat4("view", view);
    m_gridProgram->UploadUniformMat4("projection", projection);
    m_quadVAO->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_quadVAO->Unbind();
    glUseProgram(0);

    glEnable(GL_DEPTH_TEST);

    //render meshs in scene
    for (int i = 0; i < scene.meshes.size(); i++) {
        Rendermesh(scene.meshes[i], scene.camera, view, projection);
    }

    //render splinesb nm
    for (int i = 0; i < scene.splines.size(); i++) {
        RenderSpline(scene.splines[i], scene.camera, view, projection);
    }
}


void GraphicsPipeline::PresentScene() {
    glfwSwapBuffers(p_window->GetGLFWWindow());
}

void GraphicsPipeline::CleanUp() {
    delete m_unlitProgram;
    delete m_checkersProgram;
    delete m_gridProgram;
    delete m_quadVAO;
    delete m_normalProgram;
    delete m_splineProgram;
}

