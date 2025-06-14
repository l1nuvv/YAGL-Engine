//
// Created by l1nuvv on 08.06.2025.
//

#include "Renderer.h"
#include "../utils/Logger.h"
#include "GLFW/glfw3.h"

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
    glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_FILL : GL_LINE);
    CheckGLError("SetWireframeMode");
}

GLuint Renderer::CreateShader(const std::string &vertexSource, const std::string &fragmentSource)
{
    GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) return 0;

    GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = LinkShaderProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Renderer::DeleteShader(GLuint shaderProgram)
{
    glDeleteProgram(shaderProgram);
    CheckGLError("DeleteShader");
}

GLuint Renderer::LoadShaderFromFiles(const std::string &vertexPath, const std::string &fragmentPath)
{
    // Логируем попытку загрузки шейдеров
    LOG_DEBUG("Loading shaders: {} and {}", vertexPath, fragmentPath);

    // Проверяем существование файлов
    if (!ValidateShaderFile(vertexPath, "Vertex") || !ValidateShaderFile(fragmentPath, "Fragment")) {
        return 0;
    }

    std::string vertexSource   = ReadFile(vertexPath);
    std::string fragmentSource = ReadFile(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty()) {
        LOG_ERROR("Failed to read shader files: {} or {}", vertexPath, fragmentPath);
        return 0;
    }

    LOG_DEBUG("Successfully loaded shader files, creating shader program...");
    return CreateShader(vertexSource, fragmentSource);
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

GLuint Renderer::CompileShader(const std::string &source, GLenum type)
{
    GLuint      shader = glCreateShader(type);
    const char *src    = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::string shaderType = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
        LOG_ERROR("{} shader compilation failed: {}", shaderType, infoLog);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint Renderer::LinkShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG_ERROR("Shader program linking failed: {}", infoLog);

        glDeleteProgram(program);
        return 0;
    }

    return program;
}

std::string Renderer::ReadFile(const std::string &filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open file {}", filepath);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

bool Renderer::ValidateShaderFile(const std::string &path, const std::string &shaderType)
{
    if (!std::filesystem::exists(path)) {
        LOG_ERROR("{} shader file does not exist: {}", shaderType, path);
        return false;
    }
    return true;
}
