//
// Created by l1nuvv on 08.06.2025.
//

#include "Renderer.h"
#include "../utils/Logger.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

Renderer::Renderer() {}

Renderer::~Renderer() { Shutdown(); }

bool Renderer::Initialize()
{
    if (m_initialized) {
        LOG_WARN("Renderer already initialized");
        return true;
    }

    glEnable(GL_DEPTH_TEST);

    CheckGLError("Renderer initialization");

    m_initialized = true;
    LOG_INFO("Renderer initialized successfully");
    return true;
}

void Renderer::Shutdown()
{
    if (!m_initialized) { return; }

    LOG_INFO("Shutting down Renderer");
    m_initialized = false;
}

void Renderer::Clear(const glm::vec4 &clearColor)
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetViewport(int width, int height)
{
    glViewport(0, 0, width, height);
    CheckGLError("SetViewport");
}

void Renderer::SetWireframeMode(bool enabled)
{
    glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
    CheckGLError("SetWireframeMode");
}

// Объект меняет цвет с заданного на черный и обратно
void Renderer::AnimateColorPulse(GLuint shaderProgram, const glm::vec3 &baseColor, GLfloat speed)
{
    GLfloat timeValue = glfwGetTime() * speed;
    GLfloat intensity = 0.5f * (1.0f + sin(timeValue * speed));

    glm::vec3 dynamicColor        = baseColor * intensity;
    GLint     vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation,
                dynamicColor.r,
                dynamicColor.g,
                dynamicColor.b,
                1.0f);
}

void Renderer::SetShaderGradient(GLuint shaderProgram, const glm::vec3 &colorStart, const glm::vec3 &colorEnd,
                                 float  speed)
{
    GLint timeLocation  = glGetUniformLocation(shaderProgram, "time");
    GLint startLocation = glGetUniformLocation(shaderProgram, "colorStart");
    GLint endLocation   = glGetUniformLocation(shaderProgram, "colorEnd");

    glUniform1f(timeLocation, glfwGetTime() * speed);
    glUniform3f(startLocation, colorStart.r, colorStart.g, colorStart.b);
    glUniform3f(endLocation, colorEnd.r, colorEnd.g, colorEnd.b);
}

GLuint Renderer::CreateVAO()
{
    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    CheckGLError("Create VAO");
    return VAO;
}

GLuint Renderer::CreateVBO(const void *data, size_t size, GLenum usage)
{
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("Create VBO");
    return VBO;
}

GLuint Renderer::CreateEBO(const void *data, size_t count, GLenum usage)
{
    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    CheckGLError("Create EBO");
    return EBO;
}

void Renderer::SetModelMatrix(GLuint shaderProgram, const glm::mat4 &model)
{
    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
    if (modelLocation != -1) { glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model)); }
    CheckGLError("SetModelMatrix");
}

void Renderer::SetViewMatrix(GLuint shaderProgram, const glm::mat4 &view)
{
    GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
    if (viewLocation != -1) { glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view)); }
    CheckGLError("SetViewMatrix");
}

void Renderer::SetProjectionMatrix(GLuint shaderProgram, const glm::mat4 &projection)
{
    GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    if (projectionLocation != -1) { glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection)); }
    CheckGLError("SetProjectionMatrix");
}

void Renderer::SetMVPMatrices(GLuint           shaderProgram, const glm::mat4 &model, const glm::mat4 &view,
                              const glm::mat4 &projection)
{
    SetModelMatrix(shaderProgram, model);
    SetViewMatrix(shaderProgram, view);
    SetProjectionMatrix(shaderProgram, projection);
}

void Renderer::DeleteVAO(GLuint VAO)
{
    glDeleteVertexArrays(1, &VAO);
    CheckGLError("Delete VAO");
}

void Renderer::DeleteVBO(GLuint VBO)
{
    glDeleteBuffers(1, &VBO);
    CheckGLError("Delete VBO");
}

void Renderer::DeleteEBO(GLuint EBO)
{
    glDeleteBuffers(1, &EBO);
    CheckGLError("Delete EBO");
}

void Renderer::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
    CheckGLError("DrawArrays");
}

void Renderer::DrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
    glDrawElements(mode, count, type, indices);
    CheckGLError("DrawElements");
}

void Renderer::CheckGLError(const std::string &operation)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::string message;
        switch (error) {
            case GL_INVALID_ENUM:
                message = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                message = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                message = "GL_INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                message = "GL_OUT_OF_MEMORY";
                break;
            default:
                message = "Unknown OpenGL error";
                break;
        }
        LOG_ERROR("OpenGL Error in {}: {} ({})", operation, message, error);
    }
}

