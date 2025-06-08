//
// Created by l1nuvv on 08.06.2025.
//

#include "Renderer.h"

#include <spdlog/spdlog.h>

Renderer::Renderer() {}

Renderer::~Renderer() { if (m_initialized) { Shutdown(); } }

bool Renderer::Initialize()
{
    if (m_initialized) {
        spdlog::warn("Renderer already initialized");
        return true;
    }
    if (!gladLoadGLLoader) {
        spdlog::error("Failed to initialize GLAD in Renderer::Initialize");
        return false;
    }

    glEnable(GL_DEPTH_TEST);

    CheckGLError("Renderer initialization");

    m_initialized = true;
    spdlog::info("Renderer initialized successfully");
    return true;
}

void Renderer::Shutdown()
{
    if (!m_initialized) { return; }

    spdlog::info("Shutting down Renderer");
    m_initialized = false;
}

void Renderer::Clear(const glm::vec4 &clearColor)
{
    glclear
}

