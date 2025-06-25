#include "TriangleApp.h"
#include "../engine/render/Renderer.h"
#include "../engine/utils/Logger.h"
#include "AllShaders.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
#include "render/TransformManager.h"
#include "utils/ResourceManager.h"

void TriangleApp::Initialize()
{
    LOG_INFO("Initializing Triangle Application...");

    if (!GetRenderer()) {
        LOG_ERROR("Renderer is not initialized!");
        return;
    }

    // Пытаемся использовать встроенные шейдеры через ResourceManager
    m_shaderProgram = RESOURCE_MANAGER.LoadShader(
            "triangle_shader", // Уникальное имя для кэширования
            EmbeddedShaders::TRIANGLE_VERTEX_SHADER,
            EmbeddedShaders::TRIANGLE_FRAGMENT_SHADER
            );

    if (m_shaderProgram != 0) {
        LOG_INFO("Successfully loaded embedded shaders via ResourceManager!");
    } else {
        // Fallback на файлы (если встроенные не загрузились)
        LOG_WARN("Failed to load embedded shaders, trying file paths...");

        std::vector<std::pair<std::string, std::string>> shaderPaths = {
                {"assets/shaders/triangle.vert", "assets/shaders/triangle.frag"},
                {"../assets/shaders/triangle.vert", "../assets/shaders/triangle.frag"}
        };

        for (const auto &paths: shaderPaths) {
            // Читаем шейдеры из файлов
            std::string vertexSource   = ResourceManager::ReadFile(paths.first);
            std::string fragmentSource = ResourceManager::ReadFile(paths.second);

            if (!vertexSource.empty() && !fragmentSource.empty()) {
                // Загружаем через ResourceManager
                m_shaderProgram = RESOURCE_MANAGER.LoadShader(
                        "triangle_shader_file", // Другое имя для файловых шейдеров
                        vertexSource,
                        fragmentSource
                        );

                if (m_shaderProgram != 0) {
                    LOG_INFO("Loaded shaders from files via ResourceManager: {} and {}", paths.first, paths.second);
                    break;
                }
            }
        }
    }

    if (m_shaderProgram == 0) {
        LOG_ERROR("Failed to load shaders from any source");
        return;
    }

    // =============================================
    // СОЗДАНИЕ ГЕОМЕТРИИ КВАДРАТА С ИНДЕКСАМИ
    // =============================================
    // clang-format off
    GLfloat vertices[] = {
        // Позиции          // Цвета             // Текстурные координаты
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Верхний правый
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Нижний правый
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Нижний левый
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Верхний левый
    };

    // Индексы для отрисовки квадрата из двух треугольников
    GLuint indices[] = {
        0, 1, 3,   // Первый треугольник
        1, 2, 3    // Второй треугольник
    };
    // clang-format on

    // Загрузка текстуры
    m_textureID = RESOURCE_MANAGER.LoadTexture("../assets/textures/container.jpg");
    if (m_textureID == 0) {
        LOG_ERROR("Failed to load texture from any source");
    }

    // Генерация буферов
    m_VAO = GetRenderer()->CreateVAO();
    m_VBO = GetRenderer()->CreateVBO(vertices, sizeof(vertices));

    // Создаем EBO для индексов
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Привязка VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // Привязка EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Атрибут позиций
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) 0);
    glEnableVertexAttribArray(0);

    // Атрибут цветов
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Атрибут текстуры
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) (6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    LOG_INFO("Triangle Application Initialized!");
}

void TriangleApp::Render()
{
    if (m_shaderProgram == 0 || m_VAO == 0) return;

    glUseProgram(m_shaderProgram);

    // Матрицы преобразования
    glm::mat4 model = TransformManager::CreateRotatingModel(glfwGetTime(), 1.0f);
    glm::mat4 view  = TransformManager::CreateViewMatrix(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
            );
    glm::mat4 projection = TransformManager::CreateProjectionMatrix(45.0f, 800.0f / 600.0f);

    // Установка матриц
    GetRenderer()->SetMVPMatrices(m_shaderProgram, model, view, projection);

    // Градиент
    GetRenderer()->SetShaderGradient(m_shaderProgram,
                                     glm::vec3(1.0f, 0.7f, 0.5f),
                                     glm::vec3(0.3f, 0.8f, 1.0f),
                                     1.0f
            );

    if (m_textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        GLint textureLocation = glGetUniformLocation(m_shaderProgram, "ourTexture");
        if (textureLocation != -1) {
            glUniform1i(textureLocation, 0);
        }
    }

    // Отрисовка
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // ✅ Теперь правильно используем DrawElements
    glBindVertexArray(0);
    glUseProgram(0);
}

void TriangleApp::Shutdown()
{
    LOG_INFO("Shutting down Triangle Application...");

    if (m_shaderProgram != 0) {
        // Выгружаем шейдер
        RESOURCE_MANAGER.UnloadShader("triangle_shader");      // Для встроенных шейдеров
        RESOURCE_MANAGER.UnloadShader("triangle_shader_file"); // Для файловых шейдеров
        m_shaderProgram = 0;
    }

    if (m_textureID != 0) {
        RESOURCE_MANAGER.UnloadTexture("../assets/textures/container.jpg");
        m_textureID = 0;
    }

    // Очистка геометрии
    if (m_VAO != 0) {
        GetRenderer()->DeleteVAO(m_VAO);
        m_VAO = 0;
    }

    if (m_VBO != 0) {
        GetRenderer()->DeleteVBO(m_VBO);
        m_VBO = 0;
    }

    if (m_EBO != 0) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
}
