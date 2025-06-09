//
// Created by l1nuvv on 08.06.2025.
//

#pragma once

#include <string>
#include <functional>

struct GLFWwindow;

struct WindowProps
{
    std::string title;
    int         width;
    int         height;
    bool        vsync;

    WindowProps(const std::string &title  = "YAGL Engine",
                int                width  = 1280,
                int                height = 720,
                bool               vsync  = true)
        : title(title), width(width), height(height), vsync(vsync) {}
};

class Window
{
public:
    using EventCallbackFn  = std::function<void(bool)>;
    using ResizeCallbackFn = std::function<void(int, int)>;

public:
    Window(const WindowProps &props);
    ~Window();

    void Update();
    void SwapBuffers();

    bool ShouldClose() const;
    void SetShouldClose(bool close);

    int GetWidth() const { return m_data.width; }

    int GetHeight() const { return m_data.height; }

    const std::string &       GetTitle() const { return m_data.title; }
    [[nodiscard]] GLFWwindow *GetNativeWindow() const { return m_window; }

    void SetVSync(bool enabled);
    bool IsVSyncEnabled() const { return m_data.vsync; }

    void SetResizeCallback(const ResizeCallbackFn &callback) { m_data.resizeCallback = callback; }

private:
    void Initialize(const WindowProps &props);
    void Shutdown();
    void SetupCallbacks();

private:
    GLFWwindow *m_window;

    struct WindowData
    {
        std::string      title;
        int              width, height;
        bool             vsync;
        ResizeCallbackFn resizeCallback;
    };

    WindowData m_data;
};
