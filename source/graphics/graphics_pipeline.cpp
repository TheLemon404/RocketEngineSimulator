//
// Created by Osprey on 6/4/2025.
//

#include "graphics_pipeline.h"

#include "imoguizmo.hpp"
#include "../core/input.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"

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

void GraphicsPipeline::RegisterLinePath(LinePath &linePath) {
    linePath.vao = new VertexArrayObject();
    linePath.vao->Bind();

    //vertex positions
    linePath.positionsBuffer = new BufferObject<float>();
    linePath.positionsBuffer->Upload(linePath.ExtractPositionsArray());
    linePath.vao->CreateVertexAttributePointer(0, 3, sizeof(float), GL_FLOAT);

    linePath.vao->Unbind();

    std::cout << "linePath: " << linePath.id << " has been registered" << std::endl;
}

void GraphicsPipeline::RegisterPipe(Pipe &pipe) {
    pipe.vao = new VertexArrayObject();
    pipe.vao->Bind();

    pipe.positionsBuffer = new BufferObject<float>();
    pipe.positionsBuffer->Upload(pipe.ExtractPositionsArray());
    pipe.vao->CreateVertexAttributePointer(0, 3, sizeof(float), GL_FLOAT);

    pipe.vao->Unbind();

    std::cout << "pipe: " << pipe.id << " has been registered" << std::endl;
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

    m_linePathVertexShader = new ShaderObject(GL_VERTEX_SHADER);
    m_linePathVertexShader->Load("resources/shaders/linePath.vert");
    m_linePathFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER);
    m_linePathFragmentShader->Load("resources/shaders/linePath.frag");
    m_linePathProgram = new ShaderProgramObject();
    m_linePathProgram->Compile(m_linePathVertexShader, m_linePathFragmentShader);

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

    //initialize imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(p_window->GetGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void GraphicsPipeline::RegisterScene(Scene& scene) {
    for (int i = 0; i < scene.meshes.size(); i++) {
        RegisterMesh(scene.meshes[i]);
    }
    for (int i = 0; i < scene.linePaths.size(); i++) {
        RegisterLinePath(scene.linePaths[i]);
    }
    for (int i = 0; i < scene.pipes.size(); i++) {
        RegisterPipe(scene.pipes[i]);
    }
}

void GraphicsPipeline::ClearSelection(Scene& scene) {
    for (int i = 0; i < scene.linePaths.size(); i++) {
        for (int j = 0; j < scene.linePaths[i].controls.size(); j++) {
            scene.linePaths[i].controls[j].selected = false;
        }
    }
    for (int i = 0; i < scene.pipes.size(); i++) {
        for (int j = 0; j < scene.pipes[i].path.controls.size(); j++) {
            scene.pipes[i].path.controls[j].selected = false;
        }
    }
    m_currentSelectedControlIndex = -1;
}

void GraphicsPipeline::DrawDebugSphere3D(glm::vec3 center, float radius, glm::vec3 color, Camera camera) {
    const int rings = 32;
    const int sectors = 64;

    //calculate matricies
    glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), ((float)p_window->GetWindowDimentions().x / (float)p_window->GetWindowDimentions().y), 0.001f, 10000.0f);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(projection));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(view));

    // Apply model transform (position and scale)
    glTranslatef(center.x, center.y, center.z);
    glScalef(radius, radius, radius);

    glColor3f(color.r, color.g, color.b); // Yellow wireframe

    // Draw latitude lines (horizontal rings)
    for (int i = 0; i < rings; i++) {
        float phi1 = M_PI * i / rings;       // Current latitude
        float phi2 = M_PI * (i + 1) / rings; // Next latitude

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= sectors; j++) {
            float theta = 2.0f * M_PI * j / rings; // Longitude

            // First vertex (current stack)
            float x1 = radius * sin(phi1) * cos(theta);
            float y1 = radius * cos(phi1);
            float z1 = radius * sin(phi1) * sin(theta);
            glVertex3f(x1, y1, z1);

            // Second vertex (next stack)
            float x2 = radius * sin(phi2) * cos(theta);
            float y2 = radius * cos(phi2);
            float z2 = radius * sin(phi2) * sin(theta);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    glPopMatrix(); // Restore modelview

    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); // Restore projection
    glMatrixMode(GL_MODELVIEW);
}

void GraphicsPipeline::DrawDebugCircle2D(glm::vec2 center, float radius, glm::vec3 color) {
    glColor3f(color.r, color.g, color.b);

    glPushMatrix();
    glTranslatef(center.x, center.y, 0);

    // Draw 3 circles for X, Y, Z planes

    // XY plane circle
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * M_PI * i / 32.0f;
        glVertex3f(cos(angle) * radius, sin(angle) * radius, 0.0f);
    }
    glEnd();

    // XZ plane circle
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * M_PI * i / 32.0f;
        glVertex3f(cos(angle) * radius, 0.0f, sin(angle) * radius);
    }
    glEnd();

    // YZ plane circle
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * M_PI * i / 32.0f;
        glVertex3f(0.0f, cos(angle) * radius, sin(angle) * radius);
    }
    glEnd();

    glPopMatrix();
}

void GraphicsPipeline::DrawLinePathGizmos(LinePath linePath, Camera camera) {
    for (int i = 0; i < linePath.controls.size(); i++) {
        DrawDebugSphere3D(linePath.controls[i].position, 0.2f, linePath.controls[i].selected ? glm::vec3(1.0f) : glm::vec3(1.0f, 0.0, 0.0), camera);
    }
}

void GraphicsPipeline::RenderMesh(Mesh& mesh, glm::mat4 view, glm::mat4 projection) {
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
    transform = glm::scale(transform, mesh.scale);
    transform = glm::rotate(transform, mesh.rotation.x, glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, mesh.rotation.y, glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, mesh.rotation.z, glm::vec3(0, 0, 1));
    transform = glm::translate(transform, mesh.position);

    m_unlitProgram->UploadUniformMat4("projection", projection);
    m_unlitProgram->UploadUniformMat4("view", view);
    m_unlitProgram->UploadUniformMat4("transform", transform);

    mesh.vao->Bind();
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    mesh.vao->Unbind();

    //stop using any shader program
    glUseProgram(0);
}

void GraphicsPipeline::RenderLinePath(LinePath& linePath, glm::mat4 view, glm::mat4 projection) {
    //draw curves (for debug)
    m_linePathProgram->Use();
    m_linePathProgram->UploadUniformMat4("view", view);
    m_linePathProgram->UploadUniformMat4("projection", projection);
    m_linePathProgram->UploadUniformVec4("tint", glm::vec4(1.0f));
    m_linePathProgram->UploadUniformMat4("transform", glm::identity<glm::mat4>());
    linePath.vao->Bind();
    glDrawArrays(GL_LINE_STRIP, 0, linePath.ExtractPositionsArray().size() / 3);
    linePath.vao->Unbind();
    glUseProgram(0);
}

void GraphicsPipeline::RenderPipe(Pipe &pipe, glm::mat4 view, glm::mat4 projection) {
    m_unlitProgram->Use();
    m_unlitProgram->UploadUniformMat4("projection", projection);
    m_unlitProgram->UploadUniformMat4("view", view);
    m_unlitProgram->UploadUniformMat4("transform", glm::identity<glm::mat4>());
    pipe.vao->Bind();
    glDrawArrays(GL_LINE_STRIP, 0, pipe.ExtractPositionsArray().size() / 3);
    pipe.vao->Unbind();
    glUseProgram(0);
}

void GraphicsPipeline::UpdateGeometry(Scene &scene) {
    //calculate matricies
    glm::mat4 view = glm::lookAt(scene.camera.position, scene.camera.target, scene.camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(scene.camera.fov), ((float)p_window->GetWindowDimentions().x / (float)p_window->GetWindowDimentions().y), 0.001f, 10000.0f);

    //calculate world space mouse position for object moving
    glm::vec3 planeNormal = glm::normalize(scene.camera.target - scene.camera.position);
    glm::vec3 planePosition;
    if (m_isLineSelected) planePosition = m_currentSelectedControlIndex != -1 ? scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].position : glm::vec3(0.0);
    else planePosition = m_currentSelectedControlIndex != -1 ? scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].position : glm::vec3(0.0);
    glm::vec2 ndc = glm::vec2((2.0f * Input::mousePosition.x) / p_window->GetWindowDimentions().x - 1.0f, 1.0f - (2.0 * Input::mousePosition.y) / p_window->GetWindowDimentions().y);
    glm::mat4 inverseViewProjectionMatrix = glm::inverse(projection * view);
    glm::vec4 nearClip = {ndc.x, ndc.y, -1.0f, 1.0f};
    glm::vec4 farClip = {ndc.x, ndc.y, 0.0f, 1.0f};
    glm::vec4 nearWorld = inverseViewProjectionMatrix * nearClip;
    glm::vec4 farWorld = inverseViewProjectionMatrix * farClip;
    glm::vec3 near = glm::vec3(nearWorld) / nearWorld.w;
    glm::vec3 far = glm::vec3(farWorld) / farWorld.w;
    glm::vec3 worldPos;
    glm::vec3 rayDir = glm::normalize(glm::vec3(far - near));
    glm::vec3 rayOrigin = glm::vec3(near);

    // Step 5: Intersect ray with plane
    float denominator = glm::dot(planeNormal, rayDir);
    if (abs(denominator) < 1e-6) {
        // Ray is parallel to plane
        worldPos = glm::vec3(0,0,0);
    }

    float t = glm::dot(planeNormal, (planePosition - rayOrigin)) / denominator;
    worldPos = rayOrigin + t * rayDir;

    //manage gizmo selection and linePath manipulation
    if (Input::IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
        ClearSelection(scene);
        for (int i = 0; i < scene.pipes.size(); i++) {
            m_currentSelectedControlIndex = scene.pipes[i].path.GetSelectedControlIndex(Input::mousePosition, view, projection, p_window->GetWindowDimentions());
            if(m_currentSelectedControlIndex != -1) {
                m_currentSelectedPipeIndex = i;
                m_isLineSelected = false;
                return;
            }
        }
        for (int i = 0; i < scene.linePaths.size(); i++) {
            m_currentSelectedControlIndex = scene.linePaths[i].GetSelectedControlIndex(Input::mousePosition, view, projection, p_window->GetWindowDimentions());
            if(m_currentSelectedControlIndex != -1) {
                m_currentSelectedLinePathIndex = i;
                m_isLineSelected = true;
                return;
            }
        }
    }

    if (Input::mouseButtonStates[GLFW_MOUSE_BUTTON_1] == GLFW_PRESS && m_currentSelectedControlIndex != -1) {
        if (Input::keyStates[GLFW_KEY_LEFT_SHIFT] == GLFW_RELEASE) {
            if (m_isLineSelected) {
                scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].position = worldPos;

                scene.linePaths[m_currentSelectedLinePathIndex].UpdatePositionsBuffer();
                if (m_currentSelectedLinePathIndex - 1 >= 0) scene.linePaths[m_currentSelectedLinePathIndex - 1].UpdatePositionsBuffer();
                if (m_currentSelectedLinePathIndex + 1 < scene.linePaths.size()) scene.linePaths[m_currentSelectedLinePathIndex + 1].UpdatePositionsBuffer();
            }
            else {
                scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].position = worldPos;

                scene.pipes[m_currentSelectedPipeIndex].UpdatePositionsBuffer();
                if (m_currentSelectedLinePathIndex - 1 >= 0) scene.linePaths[m_currentSelectedLinePathIndex - 1].UpdatePositionsBuffer();
                if (m_currentSelectedLinePathIndex + 1 < scene.linePaths.size()) scene.linePaths[m_currentSelectedLinePathIndex + 1].UpdatePositionsBuffer();
            }
        }
        else {
            if (m_isLineSelected) {
                glm::vec3 origin = scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].position;
                glm::vec3 delta = worldPos - origin;
                glm::vec3 axis = LinePath::RoundToMajorAxis(abs(glm::normalize(worldPos - origin)));
                scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].position = origin + (delta * axis);

                scene.linePaths[m_currentSelectedLinePathIndex].UpdatePositionsBuffer();
                if (m_currentSelectedLinePathIndex - 1 >= 0) scene.linePaths[m_currentSelectedLinePathIndex - 1].UpdatePositionsBuffer();
                if (m_currentSelectedLinePathIndex + 1 < scene.linePaths.size()) scene.linePaths[m_currentSelectedLinePathIndex + 1].UpdatePositionsBuffer();
            }
            else {
                glm::vec3 origin = scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].position;
                glm::vec3 delta = worldPos - origin;
                glm::vec3 axis = LinePath::RoundToMajorAxis(abs(glm::normalize(worldPos - origin)));
                scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].position = origin + (delta * axis);

                scene.pipes[m_currentSelectedPipeIndex].UpdatePositionsBuffer();
                if (m_currentSelectedLinePathIndex - 1 >= 0) scene.linePaths[m_currentSelectedLinePathIndex - 1].UpdatePositionsBuffer();
                if (m_currentSelectedLinePathIndex + 1 < scene.linePaths.size()) scene.linePaths[m_currentSelectedLinePathIndex + 1].UpdatePositionsBuffer();
            }
        }
    }

    if ((Input::keyStates[GLFW_KEY_B] == GLFW_PRESS || Input::keyStates[GLFW_KEY_B] == GLFW_REPEAT) && Input::mouseScrollVector.y != 0 && m_currentSelectedControlIndex != -1) {
        if (m_isLineSelected) {
            if (m_currentSelectedControlIndex - 1 >= 0 && m_currentSelectedControlIndex + 1 < scene.linePaths[m_currentSelectedLinePathIndex].controls.size()) {
                scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].bevelNumber += Input::mouseScrollVector.y;
                scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].bevelNumber = std::clamp(scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].bevelNumber, 0, 4);
                scene.linePaths[m_currentSelectedLinePathIndex].UpdatePositionsBuffer();
            }
        }
        else {
            if (m_currentSelectedControlIndex - 1 >= 0 && m_currentSelectedControlIndex + 1 < scene.pipes[m_currentSelectedPipeIndex].path.controls.size()) {
                scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].bevelNumber += Input::mouseScrollVector.y;
                scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].bevelNumber = std::clamp(scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].bevelNumber, 0, 4);
                scene.pipes[m_currentSelectedPipeIndex].UpdatePositionsBuffer();
            }
        }
    }

    if ((Input::keyStates[GLFW_KEY_R] == GLFW_PRESS || Input::keyStates[GLFW_KEY_R] == GLFW_REPEAT) && Input::mouseScrollVector.y != 0 && m_currentSelectedControlIndex != -1) {
        if (m_isLineSelected) {
            if (m_currentSelectedControlIndex - 1 >= 0 && m_currentSelectedControlIndex + 1 < scene.linePaths[m_currentSelectedLinePathIndex].controls.size()) {
                scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].radius += Input::mouseScrollVector.y / 100.0f;
                scene.linePaths[m_currentSelectedLinePathIndex].UpdatePositionsBuffer();
            }
        }
        else {
            if (m_currentSelectedControlIndex - 1 >= 0 && m_currentSelectedControlIndex + 1 < scene.pipes[m_currentSelectedPipeIndex].path.controls.size()) {
                scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].radius += Input::mouseScrollVector.y / 100.0f;
                scene.pipes[m_currentSelectedPipeIndex].UpdatePositionsBuffer();
            }
        }
    }

    if (Input::IsKeyJustReleased(GLFW_KEY_E) && m_currentSelectedControlIndex != -1) {
        if (m_isLineSelected) {
            int newSelectedControl = scene.linePaths[m_currentSelectedLinePathIndex].Extrude(m_currentSelectedControlIndex, worldPos);
            scene.linePaths[m_currentSelectedLinePathIndex].UpdatePositionsBuffer();
            ClearSelection(scene);
            m_currentSelectedControlIndex = newSelectedControl;
            scene.linePaths[m_currentSelectedLinePathIndex].controls[m_currentSelectedControlIndex].selected = true;
        }
        else {
            int newSelectedControl = scene.pipes[m_currentSelectedPipeIndex].path.Extrude(m_currentSelectedControlIndex, worldPos);
            scene.pipes[m_currentSelectedPipeIndex].UpdatePositionsBuffer();
            ClearSelection(scene);
            m_currentSelectedControlIndex = newSelectedControl;
            scene.pipes[m_currentSelectedPipeIndex].path.controls[m_currentSelectedControlIndex].selected = true;
        }
    }
}

void GraphicsPipeline::RenderScene(Scene& scene) {
    //calculate matricies
    glm::mat4 view = glm::lookAt(scene.camera.position, scene.camera.target, scene.camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(scene.camera.fov), ((float)p_window->GetWindowDimentions().x / (float)p_window->GetWindowDimentions().y), 0.001f, 10000.0f);

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
    m_gridProgram->UploadUniformMat4("view", view);
    m_gridProgram->UploadUniformMat4("projection", projection);
    m_quadVAO->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_quadVAO->Unbind();
    glUseProgram(0);

    glEnable(GL_DEPTH_TEST);

    //render meshes in scene
    for (int i = 0; i < scene.meshes.size(); i++) {
        RenderMesh(scene.meshes[i], view, projection);
    }

    //render linePaths in scene
    for (int i = 0; i < scene.linePaths.size(); i++) {
        RenderLinePath(scene.linePaths[i], view, projection);
        DrawLinePathGizmos(scene.linePaths[i], scene.camera);
    }

    for (int i = 0; i < scene.pipes.size(); i++) {
        RenderPipe(scene.pipes[i], view, projection);
        DrawLinePathGizmos(scene.pipes[i].path, scene.camera);
    }
}


void GraphicsPipeline::DrawUI(Scene& scene) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //orientation guizmo
    glm::mat4 view;
    view = glm::lookAt(scene.camera.position, scene.camera.target, scene.camera.up);
    ImOGuizmo::SetRect(p_window->GetWindowDimentions().x - 150, 60, 100.0f);
    ImOGuizmo::BeginFrame();
    glm::mat4 projection = glm::perspective(glm::radians(scene.camera.fov), (float)p_window->GetWindowDimentions().y/p_window->GetWindowDimentions().x, 0.001f, 10000.0f);
    if (!ImOGuizmo::DrawGizmo((float*)&view, (float*)&projection)){
        //maybe implement something here later
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(p_window->GetGLFWWindow());
}

void GraphicsPipeline::CleanUp() {
    delete m_unlitProgram;
    delete m_checkersProgram;
    delete m_gridProgram;
    delete m_quadVAO;
    delete m_normalProgram;
    delete m_linePathProgram;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

