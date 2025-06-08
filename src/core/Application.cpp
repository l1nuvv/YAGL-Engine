//
// Created by l1nuvv on 08.06.2025.
//

#include "Application.h"
#include "Window.h"
#include "Input.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

Application *Application::s_instance = nullptr;

Application::Application(int width, int height, const std::string &title)
{
    if (s_instance) {
        spdlog::error("Application already exists!");
        return;
    }
    s_instance = this;

    spdlog::info("GLM vector size for test: {}", sizeof(glm::vec3));

    WindowProps windowProps(title, width, height, true);
    m_window = std::make_unique<Window>(windowProps);
}

Application::~Application()
{
    ShutdownEngine();
    s_instance = nullptr;
}

void Application::Run()
{
    InitializeEngine();

    if (!m_running) {
        spdlog::error("Failed to initialize application!");
        return;
    }

    spdlog::info("Running application...");

    while (m_running && m_window && !m_window->ShouldClose()) {
        CalculateDeltaTime();

        Input::ProcessInput();

        if (Input::ShouldClose()) {
            m_running = false;
            break;
        }

        Update(m_deltaTime);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Render();

        m_window->SwapBuffers();
        m_window->Update();
    }

    spdlog::info("Application exiting...");
    ShutdownEngine();
}

void Application::InitializeEngine()
{
    if (!m_window) {
        spdlog::error("Window not created!");
        m_running = false;
        return;
    }

    Input::Initialize(m_window->GetNativeWindow());

    m_window->SetResizeCallback([this](int width, int height) { OnWindowResize(width, height); });

    Initialize();

    m_running       = true;
    m_lastFrameTime = static_cast<float>(glfwGetTime());
}

void Application::ShutdownEngine()
{
    Shutdown();

    // if (m_renderer) m_renderer.reset();

    if (m_window) m_window.reset();

    m_running = false;
}

void Application::CalculateDeltaTime()
{
    auto currentTime = static_cast<float>(glfwGetTime());
    m_deltaTime      = currentTime - m_lastFrameTime;
    m_lastFrameTime  = currentTime;
}
