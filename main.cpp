#include "game/TriangleApp.h"
#include "engine/util/Logger.h"

int main()
{
    // Инициализируем логгер перед использованием
    Logger::Init();

    LOG_INFO("Starting YAGL Engine with Triangle Demo...");

    auto app = std::make_unique<TriangleApp>();
    app->Run();

    LOG_INFO("Application finished successfully!");
    return 0;
}
