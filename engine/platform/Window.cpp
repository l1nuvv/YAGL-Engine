//
// Created by l1nuvv on 08.06.2025.
//

#include "Window.h"
#include "../util/Logger.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


Window::Window(const WindowProps &props) : m_window(nullptr) { Initialize(props); }

Window::~Window() { Shutdown(); }

void Window::Initialize(const WindowProps &props)
{
    // Копирование параметров в локальное хранилище
    m_data.title  = props.title;
    m_data.width  = props.width;
    m_data.height = props.height;
    m_data.vsync  = props.vsync;

    // Инициализация GLFW(!)
    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW!");
        return;
    }

    // Настройка контекста OpenGL перед созданием окна и указание современной версии
    LOG_DEBUG("OpenGL version: 4.6(CORE_PROFILE).");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создаем окно с указанными параметрами
    LOG_INFO("Creating window by GLFW {}x{}...", m_data.width, m_data.height);
    m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);

    if (!m_window) {
        LOG_ERROR("Failed to create GLFW window!");
        glfwTerminate();
        return;
    }

    // Привязываем наши данные из user pointer WindowData для использование в callbacka'ах
    glfwSetWindowUserPointer(m_window, &m_data);

    // Делаем контекст OpenGL текущим для этого окна
    glfwMakeContextCurrent(m_window);

    // Загружаем GLAD
    LOG_INFO("Loading OpenGl functions by GLAD...");
    if (!gladLoadGLLoader((GLADloadproc) (glfwGetProcAddress))) {
        LOG_ERROR("Failed to initialize GLAD!");
        return;
    }

    // Логи для отладки
    LOG_DEBUG("OpenGL Vendor: {}", (const char *) glGetString(GL_VENDOR));
    LOG_DEBUG("OpenGL Renderer: {}", (const char *) glGetString(GL_RENDERER));
    LOG_DEBUG("OpenGL Version: {}", (const char *) glGetString(GL_VERSION));
    LOG_DEBUG("GLSL Version: {}", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

    LOG_DEBUG("Viewport set to {}x{}...", m_data.width, m_data.height);

    // VSync по умолчанию
    SetVSync(m_data.vsync);

    // Обработчик событий
    SetupCallbacks();
}

void Window::SetupCallbacks()
{
    // Callback для изменения размера framebuffer
    // Вызывается когда пользователь меняет размер окна
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow *window, int width, int height) {
        // Получаем данные из user pointer WindowData
        WindowData &data = *(WindowData *) glfwGetWindowUserPointer(window);
        // Обновляем сохраненные размеры
        data.width  = width;
        data.height = height;
        LOG_DEBUG("Framebuffer resized to {}x{}", width, height);
        // Если установлен callback, то уведомляем приложение
        if (data.resizeCallback) data.resizeCallback(width, height);
    });
}

void Window::Update() { glfwPollEvents(); } // Обработка всех накопившихся событий GLFW

void Window::SwapBuffers() { glfwSwapBuffers(m_window); }
// Меняет местами передний и задние буферы, предотвращает мерцания

bool Window::ShouldClose() const { return !IsWindowValid() || glfwWindowShouldClose(m_window); }
// Проверка флага закрытия

void Window::SetShouldClose(bool close)
{
    // Установка флага закрытия окна
    if (IsWindowValid()) { glfwSetWindowShouldClose(m_window, close ? GLFW_TRUE : GLFW_FALSE); }
}

void Window::SetVSync(bool enabled)
{
    // Управление VSync
    // 1 = ждать VBLank, 0 = unlimited FPS
    glfwSwapInterval(enabled ? 1 : 0);
    m_data.vsync = enabled;
    LOG_DEBUG("VSync: {}", enabled ? "enabled" : "disabled");
}

void Window::Shutdown()
{
    // Освобождаем ресурсы в правильном порядке
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    // Завершаем работу GLFW
    glfwTerminate();
}
