//
// Created by l1nuvv on 08.06.2025.
//

#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * Класс для управления OpenGL рендерингом
 * Абстрагирует низкоуровневые OpenGL вызовы и предоставляет удобный интерфейс
 * и управляет жизненным циклом OpenGL ресурсов
 */

class Renderer
{
public:
    Renderer();
    ~Renderer();

    bool Initialize(); // Инициализация подсистемы рендеринга - настройка OpenGL состояния
    void Shutdown(); // Освобождение ресурсов рендеринга

    // Очистка экрана указанным цветом перед отрисовкой нового кадра
    void Clear(const glm::vec4& clearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));

    // Установка области отрисовки
    // Нужно для изменения размера окна
    void SetViewport(int width, int height);
    // Переключение между заливкой и Wireframe режимами отрисовки
    void SetWireframeMode(bool enabled);

    // Смена цвета объекта с заданного на черный и обратно
    void AnimateColorPulse(GLuint shaderProgram, const glm::vec3& baseColor, float speed);

    // Смена цвета объекта градиентом
    void SetShaderGradient(GLuint shaderProgram, const glm::vec3& colorStart, const glm::vec3& colorEnd, float speed);

    // создание OpenGL объектов для управления данными вершин
    // Vertex Array Object - описание структуры вершин
    GLuint CreateVAO();
    // Vertex Buffer Object - хранение вершин
    GLuint CreateVBO(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW);
    // Element Buffer Object - хранение индекса вершин
    GLuint CreateEBO(const void* data, size_t count, GLenum usage = GL_STATIC_DRAW);

    void SetModelMatrix(GLuint shaderProgram, const glm::mat4& model);
    void SetViewMatrix(GLuint shaderProgram, const glm::mat4& view);
    void SetProjectionMatrix(GLuint shaderProgram, const glm::mat4& projection);
    void SetMVPMatrices(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view,
                        const glm::mat4& projection);

    // Освобождение OpenGL объектов

    void DeleteVAO(GLuint vao);
    void DeleteVBO(GLuint vbo);
    void DeleteEBO(GLuint ebo);

    // Команды отрисовки
    // Отрисовка по массиву вершин
    void DrawArrays(GLenum mode, GLint first, GLsizei count); // Отрисовка по индексам
    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices = nullptr);

    // Проверка ошибок OpenGL для отладки
    void CheckGLError(const std::string& operation);

private:
    bool m_initialized = false; // Флаг успешной инициализации рендера
};
#endif // RENDERER_H
