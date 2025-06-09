#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace Logger {
    void Init();

#define LOG_INFO(...)  spdlog::info("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#define LOG_WARN(...)  spdlog::warn("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#define LOG_ERROR(...) spdlog::error("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#define LOG_DEBUG(...) spdlog::debug("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
}
