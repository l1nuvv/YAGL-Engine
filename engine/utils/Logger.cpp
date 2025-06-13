//
// Optimized: Improved logger configuration with better defaults
//
#include "Logger.h"

namespace Logger {
    void Init()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // Optimized: Simplified pattern - removed redundant timestamp precision for better readability
        console_sink->set_pattern("[%T] [%^%l%$] %v");

        auto logger = std::make_shared<spdlog::logger>("console", console_sink);
        spdlog::set_default_logger(logger);

        // Optimized: Set level based on build configuration
#ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug); // Show all in debug builds
#else
        spdlog::set_level(spdlog::level::info);  // Hide debug logs in release builds
#endif
    }
}
