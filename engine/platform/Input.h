//
// Created by l1nuvv on 08.06.2025.
//

#pragma once

#include <GLFW/glfw3.h>

class Input
{
public:
    static void Initialize(GLFWwindow *window);
    static void ProcessInput();
    static bool IsKeyPressed(int key);
    static bool ShouldClose();

private:
    static GLFWwindow *s_window;
    static bool        IsWindowValid();
};
