//
// Created by l1nuvv on 15.06.2025.
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <glad/glad.h>

#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    static ResourceManager &GetInstance();

    GLuint LoadShader(const std::string &name, const std::string &vertexSource, const std::string &fragmentSource);
    GLuint GetShader(const std::string &name) const;
    void   UnloadShader(const std::string &name);

    GLuint LoadTexture(const std::string &path);
    GLuint GetTexture(const std::string &path) const;
    void   UnloadTexture(const std::string &path);

    static std::string ReadFile(const std::string &path);

    void Shutdown();

private:
    ResourceManager() = default;
    ~ResourceManager();
    std::unordered_map<std::string, GLuint> m_shaders;
    std::unordered_map<std::string, GLuint> m_textures;
};

#define RESOURCE_MANAGER ResourceManager::GetInstance()

#endif // RESOURCEMANAGER_H
