//
// Created by l1nuvv on 15.06.2025.
//

#define STB_IMAGE_IMPLEMENTATION
#include "../../third_party/stb/stb_image.h"
#include "ResourceManager.h"
#include "Logger.h"

#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager instance;
    return instance;
}

void ResourceManager::Initialize(const std::string &assetsPath)
{
    m_assetsPath = assetsPath;
    LOG_INFO("Initializing ResourceManager with assets path: {}", m_assetsPath);

    // Проверяем существование папки assets
    if (!fs::exists(m_assetsPath)) {
        LOG_WARN("Assets directory {} does not exist, creating it...", m_assetsPath);
        fs::create_directories(m_assetsPath);
        fs::create_directories(m_assetsPath + "/textures");
        fs::create_directories(m_assetsPath + "/shaders");
    }

    // Сканируем доступные ресурсы
    ScanTextures();
    ScanShaders();

    LOG_INFO("ResourceManager initialized. Found {} textures, {} shaders",
             m_textureFilenames.size(), m_shaderFilenames.size());
}

void ResourceManager::ScanTextures()
{
    std::vector<std::string> texturePaths = {
            m_assetsPath + "/textures",
            "../" + m_assetsPath + "/textures"
    };

    std::vector<std::string> supportedExtensions = {".jpg", ".jpeg", ".png", ".bmp", ".tga"};

    for (const auto &texturePath: texturePaths) {
        if (!fs::exists(texturePath)) continue;

        try {
            for (const auto &entry: fs::recursive_directory_iterator(texturePath)) {
                if (!entry.is_regular_file()) continue;

                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                if (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.
                    end()) {
                    std::string filename = entry.path().filename().string();
                    std::string fullPath = entry.path().string();

                    // Нормализуем разделители путей
                    std::replace(fullPath.begin(), fullPath.end(), '\\', '/');

                    m_textureFilenames[filename] = fullPath;
                    LOG_DEBUG("Found texture: {} -> {}", filename, fullPath);
                }
            }
        } catch (const fs::filesystem_error &e) {
            LOG_WARN("Error scanning texture directory {}: {}", texturePath, e.what());
        }
    }
}

void ResourceManager::ScanShaders()
{
    std::vector<std::string> shaderPaths = {
            m_assetsPath + "/shaders",
            "../" + m_assetsPath + "/shaders"
    };

    std::vector<std::string> supportedExtensions = {".vert", ".frag", ".geom", ".comp"};

    for (const auto &shaderPath: shaderPaths) {
        if (!fs::exists(shaderPath)) continue;

        try {
            for (const auto &entry: fs::recursive_directory_iterator(shaderPath)) {
                if (!entry.is_regular_file()) continue;

                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                if (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.
                    end()) {
                    std::string filename = entry.path().filename().string();
                    std::string fullPath = entry.path().string();

                    // Нормализуем разделители путей
                    std::replace(fullPath.begin(), fullPath.end(), '\\', '/');

                    m_shaderFilenames[filename] = fullPath;
                    LOG_DEBUG("Found shader: {} -> {}", filename, fullPath);
                }
            }
        } catch (const fs::filesystem_error &e) {
            LOG_WARN("Error scanning shader directory {}: {}", shaderPath, e.what());
        }
    }
}

std::string ResourceManager::FindTexturePath(const std::string &filename) const
{
    auto it = m_textureFilenames.find(filename);
    if (it != m_textureFilenames.end()) {
        return it->second;
    }

    // Если не найден, попробуем найти без учета регистра
    std::string lowerFilename = filename;
    std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);

    for (const auto &[file, path]: m_textureFilenames) {
        std::string lowerFile = file;
        std::transform(lowerFile.begin(), lowerFile.end(), lowerFile.begin(), ::tolower);
        if (lowerFile == lowerFilename) {
            return path;
        }
    }

    return "";
}

std::string ResourceManager::FindShaderPath(const std::string &filename) const
{
    auto it = m_shaderFilenames.find(filename);
    return (it != m_shaderFilenames.end()) ? it->second : "";
}

std::string ResourceManager::ExtractFilename(const std::string &path) const
{
    size_t lastSlash = path.find_last_of("/\\");
    return (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
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
        glDeleteShader(vertexShader);
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
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG_ERROR("Program linking failed: {}", infoLog);
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
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
        glDeleteProgram(it->second);
        m_shaders.erase(it);
        LOG_INFO("Shader {} unloaded", name);
    } else {
        LOG_WARN("Shader {} not found for unloading", name);
    }
}

GLuint ResourceManager::LoadTexture(const std::string &filename)
{
    // Проверяем, не загружена ли уже текстура с таким именем
    if (m_textures.find(filename) != m_textures.end()) {
        LOG_WARN("Texture {} already loaded", filename);
        return m_textures[filename];
    }

    // Ищем полный путь к файлу
    std::string fullPath = FindTexturePath(filename);
    if (fullPath.empty()) {
        LOG_ERROR("Texture file {} not found in assets directories", filename);
        return 0;
    }

    LOG_INFO("Loading texture: {} from {}", filename, fullPath);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        LOG_ERROR("Failed to load texture {}: {}", fullPath, stbi_failure_reason());
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_textures[filename] = texture;
    LOG_INFO("Texture {} loaded successfully ({}x{}, {} channels)", filename, width, height, channels);
    return texture;
}

GLuint ResourceManager::GetTexture(const std::string &filename) const
{
    auto it = m_textures.find(filename);
    if (it == m_textures.end()) {
        LOG_ERROR("Texture {} not found", filename);
        return 0;
    }
    return it->second;
}

void ResourceManager::UnloadTexture(const std::string &filename)
{
    auto it = m_textures.find(filename);
    if (it != m_textures.end()) {
        glDeleteTextures(1, &it->second);
        m_textures.erase(it);
        LOG_INFO("Texture {} unloaded", filename);
    } else {
        LOG_WARN("Texture {} not found for unloading", filename);
    }
}

std::vector<std::string> ResourceManager::GetAvailableTextures() const
{
    std::vector<std::string> textures;
    for (const auto &[filename, path]: m_textureFilenames) {
        textures.push_back(filename);
    }
    return textures;
}

void ResourceManager::PrintAvailableResources() const
{
    LOG_INFO("=== Available Resources ===");

    LOG_INFO("Textures ({}):", m_textureFilenames.size());
    for (const auto &[filename, path]: m_textureFilenames) {
        LOG_INFO("  - {}", filename);
    }

    LOG_INFO("Shaders ({}):", m_shaderFilenames.size());
    for (const auto &[filename, path]: m_shaderFilenames) {
        LOG_INFO("  - {}", filename);
    }

    LOG_INFO("========================");
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
    for (auto &[name, program]: m_shaders) {
        glDeleteProgram(program);
    }
    m_shaders.clear();

    for (auto &[filename, texture]: m_textures) {
        glDeleteTextures(1, &texture);
    }
    m_textures.clear();

    m_textureFilenames.clear();
    m_shaderFilenames.clear();

    LOG_INFO("ResourceManager shutdown completed");
}

void ResourceManager::BindTexture(const std::string &filename, GLenum textureUnit) const
{
    GLuint textureID = GetTexture(filename);
    if (textureID != 0) {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        LOG_WARN("Failed to bind texture {}: not loaded", filename);
    }
}

ResourceManager::~ResourceManager()
{
    Shutdown();
}
