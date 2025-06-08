#include "core/Application.h"
#include "core/Window.h"

#include <spdlog/spdlog.h>

class GameApplication: public Application
{
public:
    GameApplication() : Application(1200, 720, "YAGL Engine - Test") {}

    void Initialize() override { spdlog::info("Game Application Initialized!"); }

    void Update(float deltaTime) override {};

    void Render() override {};

    void Shutdown() override { spdlog::info("Game Application Shutdown!"); }

    void OnWindowResize(int width, int height) override { spdlog::info("Window resized to: {}x{}", width, height); }

    void OnKeyPressed(int key) override { spdlog::debug("Key pressed: {}", key); }
};

YAGL_MAIN(GameApplication)
