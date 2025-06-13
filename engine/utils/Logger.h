#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace Logger {
    void Init();
}

// Optimized: Simplified macro pattern and reduced file/line info for less critical logs
#define LOG_INFO(...)  spdlog::info(fmt::format(__VA_ARGS__))
#define LOG_WARN(...)  spdlog::warn("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#define LOG_ERROR(...) spdlog::error("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))

// Optimized: Debug logs keep file/line info for development, but can be easily disabled
#ifdef _DEBUG
#define LOG_DEBUG(...) spdlog::debug("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
#else
#define LOG_DEBUG(...) // No-op in release builds
#endif
#endif  // LOGGER_H
