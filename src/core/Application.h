//
// Created by l1nuvv on 08.06.2025.
//
#pragma once

#include <memory>
#include <string>

class Window;
// Forward declaration commented out until Renderer is implemented
// class Renderer;

class Application
{
public:
    Application(int width = 1200, int height = 720, const std::string &title = "YAGL Engine");
    virtual ~Application();

    void Run();

    virtual void Initialize() {}
    virtual void Update(float /*deltaTime*/) {}
    virtual void Render() {}
    virtual void Shutdown() {}

    // Event handlers
    virtual void OnWindowResize(int /*width*/, int /*height*/) {}
    virtual void OnKeyPressed(int /*key*/) {}
    virtual void OnMouseMove(float /*x*/, float /*y*/) {}

    // Getters
    Window *GetWindow() const { return m_window.get(); }
    // Renderer *GetRenderer() const { return m_renderer.get(); } // Commented out until implemented

    // Application instance (singleton pattern)
    static Application *GetInstance() { return s_instance; }

private:
    void InitializeEngine();
    void ShutdownEngine();
    void CalculateDeltaTime();

private:
    std::unique_ptr<Window> m_window;
    // std::unique_ptr<Renderer> m_renderer; // Commented out until Renderer is implemented

    bool  m_running       = true;
    float m_lastFrameTime = 0.0f;
    float m_deltaTime     = 0.0f;

    static Application *s_instance;
};

// Macro for creating main function
#define YAGL_MAIN(AppClass) \
int main() { \
auto app = std::make_unique<AppClass>(); \
app->Run(); \
return 0; \
}
