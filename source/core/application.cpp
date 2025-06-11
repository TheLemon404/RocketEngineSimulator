//
// Created by Osprey on 6/3/2025.
//

#include "application.h"

#include <numbers>

#include "input.h"

Application::Application(std::string version) {
    m_version = version;
    m_window = new Window("R.E.S " + version, 1600, 1200);
    m_graphicsPipeline = new GraphicsPipeline(m_window);
}

void Application::Initialize() {
    m_window->Initialize();
    m_graphicsPipeline->Initialize();

    //testing
    scene = {};
    scene.camera = {};

    scene.meshes.push_back(Mesh::LoadmeshFromOBJ("resources/meshes/monkey_smooth.obj"));
    scene.meshes[0].position.x = 2.0f;
    scene.linePaths.push_back({{
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        }});

    m_graphicsPipeline->RegisterScene(scene);
}

void Application::Run() {
    while (!m_window->ShouldClose()) {
        m_window->Poll();

        //update camera position
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraForward = glm::normalize(scene.camera.target - scene.camera.position);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraForward));
        glm::vec3 cameraUp = glm::normalize(glm::cross(cameraForward, cameraRight));
        if (Input::mouseButtonStates[GLFW_MOUSE_BUTTON_3] == GLFW_PRESS) {
            if (Input::keyStates[GLFW_KEY_LEFT_SHIFT] == GLFW_PRESS || Input::keyStates[GLFW_KEY_LEFT_SHIFT] == GLFW_REPEAT) {
                scene.camera.position += ((cameraRight * Input::mouseDelta.x + cameraUp * Input::mouseDelta.y) / 200.0f) * (float)pow(scene.camera.zoomFactor, 5);
                scene.camera.target += ((cameraRight * Input::mouseDelta.x + cameraUp * Input::mouseDelta.y) / 200.0f) * (float)pow(scene.camera.zoomFactor,5);
            }
            else if (Input::keyStates[GLFW_KEY_LEFT_CONTROL] == GLFW_PRESS || Input::keyStates[GLFW_KEY_LEFT_CONTROL] == GLFW_REPEAT) {
                float distanceMagnifier = pow(glm::distance(scene.camera.target, scene.camera.position), 1.7f);
                scene.camera.position += ((cameraRight * Input::mouseDelta.x + cameraForward * Input::mouseDelta.y) / 200.0f) * distanceMagnifier;
                scene.camera.target += ((cameraRight * Input::mouseDelta.x + cameraForward * Input::mouseDelta.y) / 200.0f) * distanceMagnifier;
            }
            else {
                scene.camera.RotateAround(-Input::mouseDelta.x / 500.0f, glm::vec3(0, 1, 0), scene.camera.target);
                scene.camera.RotateAround(Input::mouseDelta.y / 500.0f, cameraRight, scene.camera.target);
                scene.camera.rotation.x = glm::clamp(scene.camera.rotation.x, -glm::pi<float>(), glm::pi<float>());
                scene.camera.rotation.z = glm::clamp(scene.camera.rotation.z, -glm::pi<float>(), glm::pi<float>());
            }
        }
        //to ensure that we do not zoom while beveling an edge
        if (Input::keyStates[GLFW_KEY_B] == GLFW_RELEASE && Input::keyStates[GLFW_KEY_S] == GLFW_RELEASE) {
            scene.camera.position += (scene.camera.position - scene.camera.target) * (-Input::mouseScrollVector.y / 20.0f);
            scene.camera.zoomFactor += (-Input::mouseScrollVector.y / 1000.0f);
            scene.camera.zoomFactor = glm::clamp(scene.camera.zoomFactor, 0.000001f, 1000000.0f);
        }

        //drawing
        m_graphicsPipeline->RenderScene(scene);
        m_graphicsPipeline->DrawUI(scene);

        //reset state
        Input::Refresh();
    }
}

void Application::Close() {
    scene.CleanUp();
    m_graphicsPipeline->CleanUp();
    m_window->Close();
}

Application::~Application() {
    delete m_window;
    delete m_graphicsPipeline;
}