#include "game/TriangleApp.h"
#include "engine/util/Logger.h"

int main()
{
    // Инициализируем логгер перед использованием
    Logger::Init();

    LOG_INFO("Starting YAGL Engine with Triangle Demo...");

    try {
        auto app = std::make_unique<TriangleApp>();
        app->Run();
    } catch (const std::exception &e) {
        LOG_ERROR("Application crashed: {}", e.what());
        return -1;
    }

    LOG_INFO("Application finished successfully!");
    return 0;
}
