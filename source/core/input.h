//
// Created by Osprey on 6/3/2025.
//

#pragma once

#ifndef INPUT_H
#define INPUT_H

#include <cstdint>
#define WIN32_LEAN_AND_MEAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>

#endif //INPUT_H

class Input {
private:
    static inline std::map<int, int> m_prevKeyStates;
    static inline std::map<int, int> m_prevMouseButtonStates;
public:
    static inline glm::vec2 mouseScrollVector;
    static inline glm::vec2 mousePosition;
    static inline glm::vec2 mouseDelta;

    static inline std::map<int, int> keyStates;
    static inline std::map<int, int> mouseButtonStates;

    static void Initialize();

    static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GLFWSetCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static bool IsMouseButtonJustPressed(int button);
    static bool IsMouseButtonJustReleased(int button);
    static bool IsKeyJustPressed(int key);
    static bool IsKeyJustReleased(int key);

    static void Refresh();
};
