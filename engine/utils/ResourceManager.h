//
// Created by l1nuvv on 15.06.2025.
//
// Обновленный ResourceManager с поддержкой встроенных текстур

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

class ResourceManager
{
public:
    static ResourceManager& GetInstance();

    // Инициализация с автоматическим сканированием папок
    void Initialize(const std::string& assetsPath = "assets");
    // Шейдеры
    GLuint LoadShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    GLuint GetShader(const std::string& name) const;
    void UnloadShader(const std::string& name);

    // Текстуры - теперь поддерживают как файлы, так и встроенные данные
    GLuint LoadTexture(const std::string& filename); // Сначала ищет встроенные, потом файлы
    GLuint LoadTextureFromFile(const std::string& filename); // Принудительно из файла
    GLuint LoadTextureFromMemory(const std::string& name, const unsigned char* data, size_t size); // Из памяти
    GLuint GetTexture(const std::string& filename) const;
    void UnloadTexture(const std::string& filename);
    // Утилиты
    static std::string ReadFile(const std::string& path);
    void Shutdown();
    void BindTexture(const std::string& filename, GLenum textureUnit) const;
    // Информация о доступных ресурсах
    std::vector<std::string> GetAvailableTextures() const;
    void PrintAvailableResources() const;

private:
    ResourceManager() = default;
    ~ResourceManager();
    // Сканирование папок
    void ScanTextures();
    void ScanShaders();
    // Поиск файлов
    std::string FindTexturePath(const std::string& filename) const;
    std::string FindShaderPath(const std::string& filename) const;

    // Извлечение имени файла без пути и расширения
    std::string ExtractFilename(const std::string& path) const;

    // Вспомогательная функция для загрузки текстуры из данных в памяти
    GLuint CreateTextureFromData(const unsigned char* data, int width, int height, int channels);

    std::string m_assetsPath;
    std::unordered_map<std::string, GLuint> m_shaders;
    std::unordered_map<std::string, GLuint> m_textures;

    // Карты для быстрого поиска путей по именам файлов
    std::unordered_map<std::string, std::string> m_textureFilenames; // filename -> full_path
    std::unordered_map<std::string, std::string> m_shaderFilenames; // filename -> full_path
};

#define RESOURCE_MANAGER ResourceManager::GetInstance()

#endif // RESOURCEMANAGER_H
