//
// Created by Osprey on 6/3/2025.
//

#include "application.h"

Application::Application(std::string version) {
    m_version = version;
    m_window = new Window("R.E.S " + version, 800, 600);
    m_graphicsPipeline = new GraphicsPipeline(m_window);
}

void Application::Initialize() {
    m_window->Initialize();
    m_graphicsPipeline->Initialize();
}

void Application::Run() {
    while (!m_window->ShouldClose()) {
        m_graphicsPipeline->RenderScene();
        m_graphicsPipeline->PresentScene();
        m_window->Poll();
    }
}

void Application::Close() {
    m_window->Close();
}

Application::~Application() {
    delete m_window;
    delete m_graphicsPipeline;
}