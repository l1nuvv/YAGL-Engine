#include "TriangleApp.h"
#include "../engine/render/Renderer.h"
#include "../engine/utils/Logger.h"
#include "AllShaders.h" // Подключаем все встроенные шейдеры
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
#include "render/TransformManager.h"

void TriangleApp::Initialize()
{
    LOG_INFO("Initializing Triangle Application...");

    if (!GetRenderer()) {
        LOG_ERROR("Renderer is not initialized!");
        return;
    }

    // Сначала пробуем использовать встроенные шейдеры (для релиза)
    m_shaderProgram = GetRenderer()->CreateShader(
            EmbeddedShaders::TRIANGLE_VERTEX_SHADER,
            EmbeddedShaders::TRIANGLE_FRAGMENT_SHADER
            );

    if (m_shaderProgram != 0) {
        LOG_INFO("Successfully loaded embedded shaders!");
    } else {
        // Fallback на файлы для разработки
        LOG_WARN("Failed to load embedded shaders, trying file paths...");

        std::vector<std::pair<std::string, std::string>> shaderPaths = {
                {"assets/shaders/triangle.vert", "assets/shaders/triangle.frag"},
                {"../assets/shaders/triangle.vert", "../assets/shaders/triangle.frag"},
                {"shaders/triangle.vert", "shaders/triangle.frag"},
                {"./shaders/triangle.vert", "./shaders/triangle.frag"}
        };

        for (const auto &paths: shaderPaths) {
            m_shaderProgram = GetRenderer()->LoadShaderFromFiles(paths.first, paths.second);
            if (m_shaderProgram != 0) {
                LOG_INFO("Loaded shaders from: {} and {}", paths.first, paths.second);
                break;
            }
        }
    }

    if (m_shaderProgram == 0) {
        LOG_ERROR("Failed to load shaders from any source");
        return;
    }

    // Создаем треугольник
    GLfloat vertices[] = {
            // Позиции         // Цвета
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // Bottom Right
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom Left
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // Top

    };

    /*GLfloat texCoords[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.5f, 1.0f
    };*/


    m_VAO = GetRenderer()->CreateVAO();
    m_VBO = GetRenderer()->CreateVBO(vertices, sizeof(vertices));

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // Аттрибут с координатами
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    // Аттрибут с цветом
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    //glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    LOG_INFO("Triangle Application Initialized!");
}

void TriangleApp::Render()
{
    if (m_shaderProgram == 0 || m_VAO == 0) return;

    glUseProgram(m_shaderProgram);

    // Создаем матрицы преобразования и вращение треугольника
    glm::mat4 model = TransformManager::CreateRotatingModel(glfwGetTime(), 1.0f); // Единичная матрица

    glm::mat4 view = TransformManager::CreateViewMatrix(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
            );

    glm::mat4 projection = TransformManager::CreateProjectionMatrix(45.0f, 800.0f / 600.0f);

    // Смена цвета треугольника синусоидой
    //GetRenderer()->AnimateColorPulse(m_shaderProgram, glm::vec3(1.0f, 0.7f, 0.5f), 1.5f);

    // Передача всех матриц одним вызовом
    GetRenderer()->SetMVPMatrices(m_shaderProgram, model, view, projection);

    // Градиент на треугольнике
    GetRenderer()->SetShaderGradient(m_shaderProgram, glm::vec3(1.0f, 0.7f, 0.5f),
                                     glm::vec3(0.3f, 0.8f, 1.0f), 1.0f);

    // Сдвиг отрисованного объекта
    GLfloat xOffset       = 0.5f;
    GLint   vertexXOffset = glGetUniformLocation(m_shaderProgram, "rightPos");
    glUniform1f(vertexXOffset, xOffset);

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
