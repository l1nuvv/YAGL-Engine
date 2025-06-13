//
// Created by l1nuvv on 08.06.2025.
//

#pragma once

#include <GLFW/glfw3.h>

/**
 * Статический класс для управления пользовательским вводом
 */

class Input
{
public:
    static void Initialize(GLFWwindow *window); // Инициализация ввода привязкой к окну
    static void ProcessInput();                 // Обработка базовых событий ввода каждый кадр
    static bool IsKeyPressed(int key);          // Проверка нажатия конкретной клавиши
    static bool ShouldClose();                  // Проверка запроса на закрытие приложения

private:
    static GLFWwindow *s_window;        // Указатель на GLFW окно для получения состояние ввода
    static bool        IsWindowValid(); // Проверка валидности окна перед обращением к GLFW функциям
};
