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

void Input::Refresh() {
    mouseScrollVector = glm::vec2(0, 0);
    mouseDelta = glm::vec2(0, 0);
}