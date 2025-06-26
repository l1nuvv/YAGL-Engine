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

    // Инициализируем ResourceManager с автоматическим сканированием
    RESOURCE_MANAGER.Initialize("assets"); // или "YAGL-Engine/assets"

    // Показываем доступные ресурсы (для отладки)
    RESOURCE_MANAGER.PrintAvailableResources();

    // Пытаемся использовать встроенные шейдеры через ResourceManager
    m_shaderProgram = RESOURCE_MANAGER.LoadShader(
            "triangle_shader", // Уникальное имя для кэширования
            EmbeddedShaders::TRIANGLE_VERTEX_SHADER,
            EmbeddedShaders::TRIANGLE_FRAGMENT_SHADER
            );

    if (m_shaderProgram != 0) {
        LOG_INFO("Successfully loaded embedded shaders via ResourceManager!");
    } else {
        LOG_ERROR("Failed to load embedded shaders!");
        return;
    }

    RESOURCE_MANAGER.LoadTexture("container.jpg");
    RESOURCE_MANAGER.LoadTexture("awesomeface.png");


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

    // Привязываем текстуру, если она загружена
    RESOURCE_MANAGER.BindTexture("container.jpg", GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "ourTexture1"), 0);

    RESOURCE_MANAGER.BindTexture("awesomeface.png", GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "ourTexture2"), 1);
    // Отрисовка
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void TriangleApp::Shutdown()
{
    LOG_INFO("Shutting down Triangle Application...");

    if (m_shaderProgram != 0) {
        RESOURCE_MANAGER.UnloadShader("triangle_shader");
        m_shaderProgram = 0;
    }

    if (m_textureID != 0) {
        // Указываем имя файла для выгрузки
        RESOURCE_MANAGER.UnloadTexture("container.jpg");
        RESOURCE_MANAGER.UnloadTexture("awesomeface.png");
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
