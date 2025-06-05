//
// Created by Osprey on 6/3/2025.
//

#include "window.h"

#include "input.h"
#include "glad/glad.h"

Window::Window(std::string title, unsigned int width, unsigned int height) {
    m_title = title;
    m_width = width;
    m_height = height;
    m_glfwWindow = nullptr;
}

void Window::Initialize() {
    //initialize glfw
    glfwInit();
    m_glfwWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwSetWindowUserPointer(m_glfwWindow, this);

    //callbacks
    glfwSetWindowSizeCallback(m_glfwWindow, GlfwWindowResizedCallback);
    glfwSetErrorCallback(Window::GlfwErrorCallback);

    //initialize input
    Input::Initialize();
    glfwSetKeyCallback(m_glfwWindow, Input::GLFWKeyCallback);
    glfwSetMouseButtonCallback(m_glfwWindow, Input::GLFWMouseButtonCallback);
    glfwSetScrollCallback(m_glfwWindow, Input::GLFWScrollCallback);
    glfwSetCursorPosCallback(m_glfwWindow, Input::GLFWSetCursorPosCallback);

    //set glfw context
    glfwMakeContextCurrent(m_glfwWindow);
    glfwSwapInterval(0);
}

void Window::GlfwErrorCallback(int code, const char* description)
{
    std::cerr << "GLFW Error " << code << ": " << description << std::endl;
}

void Window::GlfwWindowResizedCallback(GLFWwindow *window, int width, int height) {
    auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    win->m_width = width;
    win->m_height = height;
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(m_glfwWindow);
}

void Window::Poll() {
    glfwPollEvents();
}

void Window::Close() {
    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}