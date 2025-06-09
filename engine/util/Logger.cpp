#include "Logger.h"

namespace Logger {
    void Init()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%T.%e] [%^%l%$] %v"); // цветной лог-уровень

        auto logger = std::make_shared<spdlog::logger>("console", console_sink);
        spdlog::set_default_logger(logger);

        spdlog::set_level(spdlog::level::debug); // показываем всё: debug, info, warn, error
    }
}
