//
// Created by l1nuvv on 15.06.2025.
//

#define STB_IMAGE_IMPLEMENTATION
#include "../../third_party/stb/stb_image.h"
#include "ResourceManager.h"
#include "Logger.h"

#include <fstream>

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager instance;
    return instance;
}

GLuint ResourceManager::LoadShader(const std::string &name, const std::string &vertexSource,
                                   const std::string &fragmentSource)
{
    if (m_shaders.find(name) != m_shaders.end()) {
        LOG_WARN("Shader {} already loaded", name);
        return m_shaders[name];
    }

    GLuint      vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vSource      = vertexSource.c_str();
    glShaderSource(vertexShader, 1, &vSource, nullptr);
    glCompileShader(vertexShader);

    GLint success = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        LOG_ERROR("Vertex shader compilation failed: {}", infoLog);
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint      fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fSource        = fragmentSource.c_str();
    glShaderSource(fragmentShader, 1, &fSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        LOG_ERROR("Fragment shader compilation failed: {}", infoLog);
        glDeleteShader(fragmentShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(program, 512, nullptr, infoLog);
        LOG_ERROR("Program linking failed: {}", infoLog);
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_shaders[name] = program;
    LOG_INFO("Shader {} loaded and cached", name);
    return program;
}

GLuint ResourceManager::GetShader(const std::string &name) const
{
    auto it = m_shaders.find(name);
    if (it == m_shaders.end()) {
        LOG_ERROR("Shader {} not found", name);
        return 0;
    }
    return it->second;
}

void ResourceManager::UnloadShader(const std::string &name)
{
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        glDeleteProgram(it->second); // Программа удаляется через glDeleteProgram
        m_shaders.erase(it);
        LOG_INFO("Shader {} unloaded", name);
    } else {
        LOG_WARN("Shader {} not found for unloading", name);
    }
}

GLuint ResourceManager::LoadTexture(const std::string &path)
{
    if (m_textures.find(path) != m_textures.end()) {
        LOG_WARN("Texture {} already loaded", path);
        return m_textures[path];
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        LOG_ERROR("Failed to load texture {}", path);
        return 0;
    }

    GLenum format;
    switch (channels) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            LOG_ERROR("Unsupported texture format: {} channels", channels);
            stbi_image_free(data);
            return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_textures[path] = texture;
    LOG_INFO("Texture {} loaded ({}x{}, {} channels)", path, width, height, channels);
    return texture;
}

GLuint ResourceManager::GetTexture(const std::string &path) const
{
    auto it = m_textures.find(path);
    if (it == m_textures.end()) {
        LOG_ERROR("Texture {} not found", path);
        return 0;
    }
    return it->second;
}

void ResourceManager::UnloadTexture(const std::string &path)
{
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        glDeleteTextures(1, &it->second);
        m_textures.erase(it);
        LOG_INFO("Texture {} unloaded", path);
    } else {
        LOG_WARN("Texture {} not found for unloading", path);
    }
}

std::string ResourceManager::ReadFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open file {}", path);
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void ResourceManager::Shutdown()
{
    for (auto &[name, program]: m_shaders) { glDeleteProgram(program); }
    m_shaders.clear();

    for (auto &[path, texture]: m_textures) { glDeleteTextures(1, &texture); }
    m_textures.clear();

    LOG_INFO("ResourceManager shutdown completed");
}

ResourceManager::~ResourceManager() { Shutdown(); }
