//
// Created by l1nuvv on 08.06.2025.
//

#pragma once

#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
    Renderer();
    ~Renderer();

    bool Initialize();
    void Shutdown();

    void Clear(const glm::vec4 &clearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
    void Present();

    void SetViewport(int width, int height);
    void SetWireframeMode(bool enabled);

    GLuint CreateSahder(const std::string &vertexSource, const std::string &fragmentSource);
    void   DeleteShader(GLuint shaderProgram);

    GLuint CreateVAO();
    GLuint CreateVBO(const void *data, size_t size, GLenum usage = GL_STATIC_DRAW);
    GLuint CreateEBO(const void *data, size_t count, GLenum usage = GL_STATIC_DRAW);
    void   DeleteVAO(GLuint vao);
    void   DeleteVBO(GLuint vbo);
    void   DeleteEBO(GLuint ebo);

    void DrawArrays(GLenum mode, GLint first, GLsizei count);
    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices = nullptr);

    void CheckGLError(const std::string &operation);

private:
    GLuint CompileShader(const std::string &source, GLenum type);
    GLuint LinkShaderProgram(GLuint vertextShader, GLuint fragmentShader);

private:
    bool m_initialized = false;
};
