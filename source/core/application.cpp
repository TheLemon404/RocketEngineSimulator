//
// Created by Osprey on 6/3/2025.
//

#include "application.h"

#include <numbers>

#include "input.h"
#include "../simulation/engine_simulation.h"

Application::Application(std::string version) {
    m_version = version;
    m_window = new Window("R.E.S " + version, 1600, 1200);
    m_graphicsPipeline = new GraphicsPipeline(m_window);
    m_simulationPipeline = new SimulationPipeline();
}

void Application::Initialize() {
    m_window->Initialize();
    m_graphicsPipeline->Initialize();
    m_simulationPipeline->Initialize();

    //testing
    m_scene = new Scene();
    m_scene->camera = {};

    m_scene->models.push_back(new Tank({}, 10.0f, 10.0f));
    m_scene->models.push_back(new ElectricPump());
    m_scene->models[0]->position.x = 2.0f;
    m_scene->models[1]->position.x = -2.0f;

    m_scene->pipes.push_back(new Pipe({{
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        }}));

    m_graphicsPipeline->RegisterScene(m_scene);
    m_simulationPipeline->RegisterScene(m_scene);
}

void Application::MoveCamera() {
        //update camera position
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraForward = glm::normalize(m_scene->camera.target - m_scene->camera.position);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraForward));
        glm::vec3 cameraUp = glm::normalize(glm::cross(cameraForward, cameraRight));
        if (Input::mouseButtonStates[GLFW_MOUSE_BUTTON_3] == GLFW_PRESS) {
            if (Input::keyStates[GLFW_KEY_LEFT_SHIFT] == GLFW_PRESS || Input::keyStates[GLFW_KEY_LEFT_SHIFT] == GLFW_REPEAT) {
                m_scene->camera.position += ((cameraRight * Input::mouseDelta.x + cameraUp * Input::mouseDelta.y) / 200.0f) * (float)pow(m_scene->camera.zoomFactor, 5);
                m_scene->camera.target += ((cameraRight * Input::mouseDelta.x + cameraUp * Input::mouseDelta.y) / 200.0f) * (float)pow(m_scene->camera.zoomFactor,5);
            }
            else if (Input::keyStates[GLFW_KEY_LEFT_CONTROL] == GLFW_PRESS || Input::keyStates[GLFW_KEY_LEFT_CONTROL] == GLFW_REPEAT) {
                float distanceMagnifier = pow(glm::distance(m_scene->camera.target, m_scene->camera.position), 1.7f);
                m_scene->camera.position += ((cameraRight * Input::mouseDelta.x + cameraForward * Input::mouseDelta.y) / 200.0f) * distanceMagnifier;
                m_scene->camera.target += ((cameraRight * Input::mouseDelta.x + cameraForward * Input::mouseDelta.y) / 200.0f) * distanceMagnifier;
            }
            else {
                m_scene->camera.RotateAround(-Input::mouseDelta.x / 500.0f, glm::vec3(0, 1, 0), m_scene->camera.target);
                m_scene->camera.RotateAround(Input::mouseDelta.y / 500.0f, cameraRight, m_scene->camera.target);
                m_scene->camera.rotation.x = glm::clamp(m_scene->camera.rotation.x, -glm::pi<float>(), glm::pi<float>());
                m_scene->camera.rotation.z = glm::clamp(m_scene->camera.rotation.z, -glm::pi<float>(), glm::pi<float>());
            }
        }
        //to ensure that we do not zoom while beveling an edge
        if (Input::keyStates[GLFW_KEY_B] == GLFW_RELEASE && Input::keyStates[GLFW_KEY_R] == GLFW_RELEASE && Input::keyStates[GLFW_KEY_S] == GLFW_RELEASE) {
            m_scene->camera.position += (m_scene->camera.position - m_scene->camera.target) * (-Input::mouseScrollVector.y / 20.0f);
            m_scene->camera.zoomFactor += (-Input::mouseScrollVector.y / 1000.0f);
            m_scene->camera.zoomFactor = glm::clamp(m_scene->camera.zoomFactor, 0.000001f, 1000000.0f);
        }
}

void Application::Run() {
    while (!m_window->ShouldClose()) {
        m_window->Poll();

        MoveCamera();

        //drawing
        m_graphicsPipeline->UpdateGeometry(m_scene);
        m_simulationPipeline->StepSimulation(m_scene);
        m_graphicsPipeline->RenderScene(m_scene);
        m_graphicsPipeline->DrawUI(m_scene);

        //reset state
        Input::Refresh();
    }
}

void Application::Close() {
    m_scene->CleanUp();
    m_graphicsPipeline->CleanUp();
    m_window->Close();
}

Application::~Application() {
    delete m_scene;
    delete m_window;
    delete m_graphicsPipeline;
    delete m_simulationPipeline;
}