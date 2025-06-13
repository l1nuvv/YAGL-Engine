//
// Created by l1nuvv on 08.06.2025.
//

#include "Input.h"

#include "util/Logger.h"

GLFWwindow *Input::s_window = nullptr;

void Input::Initialize(GLFWwindow *window) { s_window = window; }
// Сохранение указателя на окно для дальнейшего использования

void Input::ProcessInput()
{
    // Обрабатываем стандартный ввод - ESC для выхода из приложения
    // Проверка нажатия ESC и установка флага закрытия окна
    if (s_window && IsKeyPressed(GLFW_KEY_ESCAPE)) {
        LOG_DEBUG("{} has been pressed", "GLFW_KEY_ESCAPE");
        glfwSetWindowShouldClose(s_window, GLFW_TRUE);
    }
}

// Если окно валидно и клавиша нажата в данный момент, то возвращает true
bool Input::IsKeyPressed(int key) { return IsWindowValid() && glfwGetKey(s_window, key) == GLFW_PRESS; }

// Проверяем что окно НЕ валидно ИЛИ установлен флаг закрытия
bool Input::ShouldClose() { return !IsWindowValid() || glfwWindowShouldClose(s_window); }

// Проверка на nullptr
bool Input::IsWindowValid() { return s_window != nullptr; }
