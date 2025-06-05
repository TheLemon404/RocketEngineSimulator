//
// Created by Osprey on 6/3/2025.
//

#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/vec2.hpp"

#endif //WINDOW_H

class Window {
private:
    unsigned int m_width;
    unsigned int m_height;
    std::string m_title;

    GLFWwindow* m_glfwWindow;

private:
    static void GlfwErrorCallback(int code, const char* description);
    static void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height);

public:
    Window(std::string title, unsigned int width, unsigned int height);

    GLFWwindow* GetGLFWWindow() { return m_glfwWindow; }
    glm::vec2 GetDimentions() { return glm::vec2(m_width, m_height); }

    void Initialize();
    bool ShouldClose();
    void Poll();

    void Close();
};