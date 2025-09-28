#include "TriangleApp.h"
#include "../engine/render/Renderer.h"
#include "../engine/utils/Logger.h"
#include "AllShaders.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
#include "render/TransformManager.h"
#include "utils/ResourceManager.h"
#include <chrono>

void TriangleApp::Initialize()
{
    LOG_INFO("Initializing Triangle Application...");

    if (!GetRenderer())
    {
        LOG_ERROR("Renderer is not initialized!");
        return;
    }

    // Инициализируем ResourceManager с автоматическим сканированием
    RESOURCE_MANAGER.Initialize("assets"); // или "YAGL-Engine/assets"

    // Показываем доступные ресурсы (для отладки)
    RESOURCE_MANAGER.PrintAvailableResources();

    // Пытаемся использовать встроенные шейдеры через ResourceManager
    m_shaderProgram = RESOURCE_MANAGER.LoadShader(
        "triangle_shader",
        // Уникальное имя для кэширования
        EmbeddedShaders::TRIANGLE_VERTEX_SHADER,
        EmbeddedShaders::TRIANGLE_FRAGMENT_SHADER
        );

    if (m_shaderProgram != 0)
    {
        LOG_INFO("Successfully loaded embedded shaders via ResourceManager!");
    } else
    {
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) 0);
    glEnableVertexAttribArray(0);

    // Атрибут цветов
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(GLfloat),
                          (void*) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Атрибут текстуры
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(GLfloat),
                          (void*) (6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    LOG_INFO("Triangle Application Initialized!");
}

void TriangleApp::Render()
{
    if (m_shaderProgram == 0 || m_VAO == 0)
    {
        LOG_ERROR("Shader program or VAO is invalid! Shader: {}, VAO: {}", m_shaderProgram, m_VAO);
        return;
    }

    glUseProgram(m_shaderProgram);

    // УПРОЩЕННЫЙ ПОДХОД: Используем только одну матрицу model для простоты
    glm::mat4 model = glm::mat4(1.0f);
    // Небольшое вращение для проверки
    model = glm::rotate(model, (float) glfwGetTime() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view           = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Установка матриц
    GLint modelLoc = glGetUniformLocation(m_shaderProgram, "model");
    GLint viewLoc  = glGetUniformLocation(m_shaderProgram, "view");
    GLint projLoc  = glGetUniformLocation(m_shaderProgram, "projection");

    if (modelLoc != -1)
    {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //LOG_DEBUG("Model matrix set");
    } else
    {
        LOG_WARN("Model uniform not found in shader");
    }

    if (viewLoc != -1)
    {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        //LOG_DEBUG("View matrix set");
    } else
    {
        LOG_WARN("View uniform not found in shader");
    }

    if (projLoc != -1)
    {
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        //LOG_DEBUG("Projection matrix set");
    } else
    {
        LOG_WARN("Projection uniform not found in shader");
    }

    // Устанавливаем время для анимации
    GLint timeLoc = glGetUniformLocation(m_shaderProgram, "time");
    if (timeLoc != -1)
    {
        glUniform1f(timeLoc, (float) glfwGetTime());
    }

    // Устанавливаем цвета для градиента
    GLint colorStartLoc = glGetUniformLocation(m_shaderProgram, "colorStart");
    GLint colorEndLoc   = glGetUniformLocation(m_shaderProgram, "colorEnd");

    if (colorStartLoc != -1)
    {
        glUniform3f(colorStartLoc, 1.0f, 0.7f, 0.5f);
    }
    if (colorEndLoc != -1)
    {
        glUniform3f(colorEndLoc, 0.3f, 0.8f, 1.0f);
    }

    // Привязываем текстуры (если они есть)
    GLint texture1Loc = glGetUniformLocation(m_shaderProgram, "ourTexture1");
    GLint texture2Loc = glGetUniformLocation(m_shaderProgram, "ourTexture2");

    if (texture1Loc != -1)
    {
        RESOURCE_MANAGER.BindTexture("container.jpg", GL_TEXTURE0);
        glUniform1i(texture1Loc, 0);
    }

    if (texture2Loc != -1)
    {
        RESOURCE_MANAGER.BindTexture("awesomeface.png", GL_TEXTURE1);
        glUniform1i(texture2Loc, 1);
    }

    LOG_INFO_THROTTLED("Model matrix: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]",
                       model[0][0],
                       model[0][1],
                       model[0][2],
                       model[0][3]);

    // Отрисовка
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void TriangleApp::Shutdown()
{
    LOG_INFO("Shutting down Triangle Application...");

    if (m_shaderProgram != 0)
    {
        RESOURCE_MANAGER.UnloadShader("triangle_shader");
        m_shaderProgram = 0;
    }

    if (m_textureID != 0)
    {
        // Указываем имя файла для выгрузки
        RESOURCE_MANAGER.UnloadTexture("container.jpg");
        RESOURCE_MANAGER.UnloadTexture("awesomeface.png");
        m_textureID = 0;
    }

    // Очистка геометрии
    if (m_VAO != 0)
    {
        GetRenderer()->DeleteVAO(m_VAO);
        m_VAO = 0;
    }

    if (m_VBO != 0)
    {
        GetRenderer()->DeleteVBO(m_VBO);
        m_VBO = 0;
    }

    if (m_EBO != 0)
    {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
}
