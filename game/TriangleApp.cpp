//
// Created by l1nuvv on 08.06.2025.
//

#include "TriangleApp.h"
#include "../engine/render/Renderer.h"
#include "../engine/util/Logger.h"

void TriangleApp::Initialize()
{
    LOG_INFO("Initializing Triangle Application...");

    if (!GetRenderer()) {
        LOG_ERROR("Renderer is not initialized!");
    }

    // Попробуем разные пути к шейдерам
    std::string vertPath = "../assets/shaders/triangle.vert";
    std::string fragPath = "../assets/shaders/triangle.frag";
    m_shaderProgram      = GetRenderer()->LoadShaderFromFiles(vertPath, fragPath);
    // Если не найдены, попробуем другие пути
    if (m_shaderProgram == 0) {
        vertPath        = "../assets/shaders/triangle.vert";
        fragPath        = "../assets/shaders/triangle.frag";
        m_shaderProgram = GetRenderer()->LoadShaderFromFiles(vertPath, fragPath);
    }

    // Еще один вариант
    if (m_shaderProgram == 0) {
        vertPath        = "../shaders/triangle.vert";
        fragPath        = "../shaders/triangle.frag";
        m_shaderProgram = GetRenderer()->LoadShaderFromFiles(vertPath, fragPath);
    }

    if (m_shaderProgram == 0) {
        LOG_ERROR("Failed to load shaders from any path");
        return;
    }

    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
    };

    m_VAO = GetRenderer()->CreateVAO();
    m_VBO = GetRenderer()->CreateVBO(vertices, sizeof(vertices));

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    LOG_INFO("Triangle Application Initialized!");
}

void TriangleApp::Render()
{
    if (m_shaderProgram == 0 || m_VAO == 0) return;

    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_VAO);
    GetRenderer()->DrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);
}

void TriangleApp::Shutdown()
{
    LOG_INFO("Shutting down Triangle Application...");

    if (m_VAO != 0) {
        GetRenderer()->DeleteVAO(m_VAO);
        m_VAO = 0;
    }

    if (m_VBO != 0) {
        GetRenderer()->DeleteVBO(m_VBO);
        m_VBO = 0;
    }

    if (m_shaderProgram != 0) {
        GetRenderer()->DeleteShader(m_shaderProgram);
        m_shaderProgram = 0;
    }
}
