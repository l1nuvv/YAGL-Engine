//
// Created by l1nuvv on 08.06.2025.
//

#include "Input.h"

GLFWwindow *Input::s_window = nullptr;

void Input::Initialize(GLFWwindow *window) { s_window = window; }

void Input::ProcessInput()
{
    if (s_window && IsKeyPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(s_window, GLFW_TRUE);
}

bool Input::IsKeyPressed(int key) { return IsWindowValid() && glfwGetKey(s_window, key) == GLFW_PRESS; }

bool Input::ShouldClose() { return !IsWindowValid() || glfwWindowShouldClose(s_window); }

bool Input::IsWindowValid() { return s_window != nullptr; }
