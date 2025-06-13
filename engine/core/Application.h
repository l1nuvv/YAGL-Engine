//
// Created by l1nuvv on 08.06.2025.
//
#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#include "glad/glad.h"
#include <memory>
#include <string>

/**
 * Базовый класс приложения - центральная точка управления жизненным циклом
 */

class Window;
class Renderer;

class Application
{
public:
    Application(int width = 1200, int height = 720, const std::string &title = "YAGL Engine");
    virtual ~Application();

    // Основной метод запуска - управляет всем жизненным циклом движка
    void Run();

    // Виртуальные методы для переопределения в наследниках
    // Позволяют кастомизировать поведение без изменения основной логики
    virtual void Initialize() {}
    virtual void Update(float /*deltaTime*/) {}
    virtual void Render() {}
    virtual void Shutdown() {}

    // Обработчики событий - реагируют на действия пользователя
    virtual void OnWindowResize(int /*width*/, int /*height*/) {}
    virtual void OnKeyPressed(int /*key*/) {}
    virtual void OnMouseMove(float /*x*/, float /*y*/) {}

    // Геттеры - дают доступ к внутренним компонентам
    Window *  GetWindow() const { return m_window.get(); }
    Renderer *GetRenderer() const { return m_renderer.get(); }

    // Синглтон, дающий глобальный доступ к приложению
    static Application *GetInstance() { return s_instance; }

private:
    // Приватные методы управления движком
    void InitializeEngine();
    void ShutdownEngine();
    void CalculateDeltaTime();

private:
    // Основные компоненты движка - умные указатели для автоматической очистки памяти
    std::unique_ptr<Window>   m_window;
    std::unique_ptr<Renderer> m_renderer;

    // Состояние приложения
    bool  m_running       = true; // Флаг продолжения работы основного цикла while
    float m_lastFrameTime = 0.0f; // Время предыдущего кадра для расчета корректного deltaTime
    float m_deltaTime     = 0.0f; // deltaTime для плавности

    // Singleton instance - статический указатель на единственный экземпляр
    static Application *s_instance;
};
#endif // APPLICATION_H
