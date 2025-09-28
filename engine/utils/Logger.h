#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>

namespace Logger
{
    void Init();
    // Throttled logging function
    template<typename... Args>
    void InfoThrottled(const char* format, Args&&... args)
    {
        static auto       last_log_time = std::chrono::steady_clock::now();
        static const auto LOG_INTERVAL  = std::chrono::seconds(1);
        auto              current_time  = std::chrono::steady_clock::now();
        if (current_time - last_log_time >= LOG_INTERVAL)
        {
            spdlog::info(format, std::forward<Args>(args)...);
            last_log_time = current_time;
        }
    }

    // Можно добавить для других уровней логирования
    template<typename... Args>
    void DebugThrottled(const char* format, Args&&... args)
    {
        static auto       last_log_time = std::chrono::steady_clock::now();
        static const auto LOG_INTERVAL  = std::chrono::seconds(1);

        auto current_time = std::chrono::steady_clock::now();
        if (current_time - last_log_time >= LOG_INTERVAL)
        {
#ifdef _DEBUG
            spdlog::debug(format, std::forward<Args>(args)...);
#endif
            last_log_time = current_time;
        }
    }
}

// Optimized: Simplified macro pattern and reduced file/line info for less critical logs
#define LOG_INFO(...)  spdlog::info(fmt::format(__VA_ARGS__))
#define LOG_WARN(...)  spdlog::warn("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#define LOG_ERROR(...) spdlog::error("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))

// Новые макросы для throttled логирования
#define LOG_INFO_THROTTLED(...) do { \
static auto last_log_time = std::chrono::steady_clock::now(); \
static const auto LOG_INTERVAL = std::chrono::seconds(1); \
auto current_time = std::chrono::steady_clock::now(); \
if (current_time - last_log_time >= LOG_INTERVAL) { \
LOG_INFO(__VA_ARGS__); \
last_log_time = current_time; \
} \
} while(0)
#define LOG_DEBUG_THROTTLED(...) Logger::DebugThrottled(__VA_ARGS__)

// Optimized: Debug logs keep file/line info for development, but can be easily disabled
#ifdef _DEBUG
#define LOG_DEBUG(...) spdlog::debug("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#else
#define LOG_DEBUG(...) // No-op in release builds
#endif

#endif  // LOGGER_H
