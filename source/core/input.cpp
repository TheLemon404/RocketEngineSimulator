//
// Created by Osprey on 6/3/2025.
//

#include "input.h"

void Input::Initialize() {
    mouseScrollVector = glm::vec2(0, 0);
    mousePosition = glm::vec2(0, 0);
}

void Input::GLFWKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    keyStates[key] = action;
}

void Input::GLFWMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    mouseButtonStates[button] = action;
}

void Input::GLFWScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    mouseScrollVector.x = xoffset;
    mouseScrollVector.y = yoffset;
}

void Input::GLFWSetCursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    mouseDelta.x = xpos - mousePosition.x;
    mouseDelta.y = ypos - mousePosition.y;
    mousePosition.x = static_cast<float>(xpos);
    mousePosition.y = static_cast<float>(ypos);
}

bool Input::IsMouseButtonJustPressed(int button) {
    if (mouseButtonStates[button] == GLFW_PRESS && m_prevMouseButtonStates[button] == GLFW_RELEASE) {
        return true;
    }

    return false;
}

bool Input::IsMouseButtonJustReleased(int button) {
    if (mouseButtonStates[button] == GLFW_RELEASE && m_prevMouseButtonStates[button == GLFW_PRESS]) {
        return true;
    }

    return false;
}

bool Input::IsKeyJustPressed(int key) {
    if (keyStates[key] == GLFW_PRESS && m_prevKeyStates[key] == GLFW_RELEASE) {
        return true;
    }

    return false;
}

bool Input::IsKeyJustReleased(int key) {
    if (keyStates[key] == GLFW_RELEASE && m_prevKeyStates[key] == GLFW_PRESS) {
        return true;
    }

    return false;
}

void Input::Refresh() {
    for (auto keyState : keyStates) {
        m_prevKeyStates[keyState.first] = keyState.second;
    }
    for (auto mouseButtonState : mouseButtonStates) {
        m_prevMouseButtonStates[mouseButtonState.first] = mouseButtonState.second;
    }
    mouseScrollVector = glm::vec2(0, 0);
    mouseDelta = glm::vec2(0, 0);
}