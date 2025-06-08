//
// Created by l1nuvv on 08.06.2025.
//

#include "Window.h"
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


Window::Window(const WindowProps &props) : m_window(nullptr) { Initialize(props); }

Window::~Window() { Shutdown(); }

void Window::Initialize(const WindowProps &props)
{
    m_data.title  = props.title;
    m_data.width  = props.width;
    m_data.height = props.height;
    m_data.vsync  = props.vsync;

    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW!");
        return;
    }

    // tell GLFW what version it should use
    spdlog::info("OpenGL version: 4.6(CORE_PROFILE).");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    spdlog::info("Creating window by GLFW {}x{}...", m_data.width, m_data.height);
    m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);

    if (!m_window) {
        spdlog::error("Failed to create GLFW window!");
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(m_window, &m_data);

    glfwMakeContextCurrent(m_window);

    // load OpenGL function pointer by using GLAD
    spdlog::info("Loading OpenGl functions by GLAD...");
    if (!gladLoadGLLoader((GLADloadproc) (glfwGetProcAddress))) {
        spdlog::error("Failed to initialize GLAD!");
        return;
    }

    // Log OpenGL info
    spdlog::info("OpenGL Vendor: {}", (const char *) glGetString(GL_VENDOR));
    spdlog::info("OpenGL Renderer: {}", (const char *) glGetString(GL_RENDERER));
    spdlog::info("OpenGL Version: {}", (const char *) glGetString(GL_VERSION));
    spdlog::info("GLSL Version: {}", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

    glViewport(0, 0, m_data.width, m_data.height);
    spdlog::debug("Viewport set to {}x{}...", m_data.width, m_data.height);

    SetVSync(m_data.vsync);

    SetupCallbacks();
}

void Window::SetupCallbacks()
{
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow *window, int width, int height) {
        WindowData &data = *(WindowData *) glfwGetWindowUserPointer(window);
        data.width       = width;
        data.height      = height;
        glViewport(0, 0, width, height);

        if (data.resizeCallback) data.resizeCallback(width, height);
    });
}

void Window::Update() { glfwPollEvents(); }

void Window::SwapBuffers() { glfwSwapBuffers(m_window); }

bool Window::ShouldClose() const { return m_window ? glfwWindowShouldClose(m_window) : true; }

void Window::SetShouldClose(bool close)
{
    if (m_window) glfwSetWindowShouldClose(m_window, close ? GLFW_TRUE : GLFW_FALSE);
}

void Window::SetVSync(bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
    m_data.vsync = enabled;
    spdlog::info("VSync: {}", enabled ? "enabled" : "disabled");
}

void Window::Shutdown()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}
