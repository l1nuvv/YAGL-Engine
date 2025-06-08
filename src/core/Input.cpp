//
// Created by l1nuvv on 08.06.2025.
//

#include "Input.h"

GLFWwindow *Input::s_window = nullptr;

void Input::Initialize(GLFWwindow *window)
{
    s_window = window;
}

void Input::ProcessInput()
{
    if (s_window && IsKeyPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(s_window, GLFW_TRUE);
}

bool Input::IsKeyPressed(int key)
{
    if (!s_window) return false;
    return glfwGetKey(s_window, key) == GLFW_PRESS;
}

bool Input::ShouldClose()
{
    if (!s_window) return true;
    return glfwWindowShouldClose(s_window);
}
