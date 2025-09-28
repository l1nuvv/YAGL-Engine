//
// Created by l1nuvv on 08.06.2025.
//

#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <functional>

struct GLFWwindow;

/**
 * Структура для передачи параметров создания окна
 */
struct WindowProps
{
    std::string title;
    int width;
    int height;
    bool vsync;

    WindowProps(const std::string& title = "YAGL Engine",
                int width = 1280,
                int height = 720,
                bool vsync = true)
        : title(title), width(width), height(height), vsync(vsync)
    {
    }
};

/**
 * Обертка вокруг GLFW окна
 * Скрывает детали работы с GLFW и предоставляет удобный C++ интерфейс
 */
class Window
{
public:
    using ResizeCallbackFn = std::function<void(int, int)>;

public:
    Window(const WindowProps& props);
    ~Window();

    // Обновление состояние окна
    void Update();
    // Показ отрисованного кадра
    void SwapBuffers();

    // Проверка запроса на закрытие окна от пользователя или системы
    bool ShouldClose() const;
    // Программная установка флага закрытия окна
    void SetShouldClose(bool close);

    // Геттеры размеров - нужны для viewport и пропорций
    int GetWidth() const { return m_data.width; }
    int GetHeight() const { return m_data.height; }
    std::pair<int, int> GetSize() const { return {m_data.width, m_data.height}; }

    const std::string& GetTitle() const { return m_data.title; }
    // Доступ к нативному GLFW окну для передачи в другие системы
    [[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_window; }

    // Управление вертикальной синхронизацией
    void SetVSync(bool enabled);
    bool IsVSyncEnabled() const { return m_data.vsync; }

    // Установка callback'а для уведомления о изменении размера окна
    void SetResizeCallback(const ResizeCallbackFn& callback) { m_data.resizeCallback = callback; }

private:
    void Initialize(const WindowProps& props);
    void Shutdown();
    // GLFW Callback для обработки событий
    void SetupCallbacks();
    // Проверка на валидонсть окна перед использованием
    bool IsWindowValid() const { return m_window != nullptr; }

private:
    GLFWwindow* m_window;

    /**
    * Структура данных окна - хранит текущее состояние
    * Передается в GLFW callback'и через user pointer
    */
    struct WindowData
    {
        std::string title;
        int width, height;
        bool vsync;
        ResizeCallbackFn resizeCallback; // Функция для уведомление о ресайзе окна
    };

    WindowData m_data;
};
#endif  // WINDOW_H
