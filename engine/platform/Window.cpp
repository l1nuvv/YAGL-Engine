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
    m_data.title  = props.title;
    m_data.width  = props.width;
    m_data.height = props.height;
    m_data.vsync  = props.vsync;

    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW!");
        return;
    }

    // tell GLFW what version it should use
    LOG_DEBUG("OpenGL version: 4.6(CORE_PROFILE).");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    LOG_INFO("Creating window by GLFW {}x{}...", m_data.width, m_data.height);
    m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);

    if (!m_window) {
        LOG_ERROR("Failed to create GLFW window!");
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(m_window, &m_data);

    glfwMakeContextCurrent(m_window);

    // load OpenGL function pointer by using GLAD
    LOG_INFO("Loading OpenGl functions by GLAD...");
    if (!gladLoadGLLoader((GLADloadproc) (glfwGetProcAddress))) {
        LOG_ERROR("Failed to initialize GLAD!");
        return;
    }

    // Log OpenGL info
    LOG_DEBUG("OpenGL Vendor: {}", (const char *) glGetString(GL_VENDOR));
    LOG_DEBUG("OpenGL Renderer: {}", (const char *) glGetString(GL_RENDERER));
    LOG_DEBUG("OpenGL Version: {}", (const char *) glGetString(GL_VERSION));
    LOG_DEBUG("GLSL Version: {}", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

    LOG_DEBUG("Viewport set to {}x{}...", m_data.width, m_data.height);

    SetVSync(m_data.vsync);

    SetupCallbacks();
}

void Window::SetupCallbacks()
{
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow *window, int width, int height) {
        WindowData &data = *(WindowData *) glfwGetWindowUserPointer(window);
        data.width       = width;
        data.height      = height;
        LOG_DEBUG("Framebuffer resized to {}x{}", width, height);
        if (data.resizeCallback) data.resizeCallback(width, height);
    });
}

void Window::Update() { glfwPollEvents(); }

void Window::SwapBuffers() { glfwSwapBuffers(m_window); }

bool Window::ShouldClose() const { return !IsWindowValid() || glfwWindowShouldClose(m_window); }

void Window::SetShouldClose(bool close)
{
    if (IsWindowValid()) { glfwSetWindowShouldClose(m_window, close ? GLFW_TRUE : GLFW_FALSE); }
}

void Window::SetVSync(bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
    m_data.vsync = enabled;
    LOG_DEBUG("VSync: {}", enabled ? "enabled" : "disabled");
}

void Window::Shutdown()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}
