//
// Created by l1nuvv on 08.06.2025.
//

#include "Application.h"
#include "../platform/Window.h"
#include "../platform/Input.h"
#include "../render/Renderer.h"
#include "../util/Logger.h"
#include <glm/glm.hpp>

Application *Application::s_instance = nullptr;

Application::Application(int width, int height, const std::string &title)
{
    if (s_instance) {
        LOG_ERROR("Application already exists!");
        return;
    }
    s_instance = this;

    LOG_DEBUG("GLM vector size for test: {}", sizeof(glm::vec3));

    WindowProps windowProps(title, width, height, true);
    m_window   = std::make_unique<Window>(windowProps);
    m_renderer = std::make_unique<Renderer>();
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
        LOG_ERROR("Failed to initialize application!");
        return;
    }

    LOG_INFO("Running application...");

    while (m_running && !m_window->ShouldClose()) {
        CalculateDeltaTime();

        Input::ProcessInput();

        if (Input::ShouldClose()) {
            m_running = false;
            break;
        }

        Update(m_deltaTime);

        m_renderer->Clear(glm::vec4(0.07f, 0.13f, 0.17f, 1.0f));

        Render();

        m_window->SwapBuffers();
        m_window->Update();
    }

    LOG_INFO("Application exiting...");
    ShutdownEngine();
}

void Application::InitializeEngine()
{
    m_running = false;

    if (!m_window) {
        LOG_ERROR("Window not created!");
        return;
    }

    Input::Initialize(m_window->GetNativeWindow());

    m_window->SetResizeCallback([this](int width, int height) {
        LOG_INFO("Window resized to {}x{}", width, height);
        OnWindowResize(width, height);
        if (m_renderer) { m_renderer->SetViewport(width, height); }
    });

    if (!m_renderer->Initialize()) {
        LOG_ERROR("Failed to initialize Renderer!");
        return;
    }

    Initialize();

    m_running       = true;
    m_lastFrameTime = static_cast<float>(glfwGetTime());
}

void Application::ShutdownEngine()
{
    Shutdown();

    if (m_renderer) m_renderer.reset();

    if (m_window) m_window.reset();

    m_running = false;
}

void Application::CalculateDeltaTime()
{
    auto currentTime = static_cast<float>(glfwGetTime());
    m_deltaTime      = currentTime - m_lastFrameTime;
    m_lastFrameTime  = currentTime;
}
