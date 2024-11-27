// Author: Jake Rieger
// Created: 11/11/2024.
//

#pragma once

#include <Types.hpp>
#include <sstream>
#include <vector>

enum class LogLevel {
    Info,
    Warning,
    Error,
    Panic,
    Debug,
};

enum class LogCategory {
    Graphics,
    Editor,
    Audio,
    Physics,
    Input,
    General,
};

static str LogLevelToString(const LogLevel& level);
static str LogCategoryToString(const LogCategory& category);

struct LogEntry {
    str Message;
    LogLevel Level;
    LogCategory Category;
    std::time_t Timestamp;

    [[nodiscard]] str ToString() const;
};

class Logger {
public:
    std::vector<LogEntry> Entries;

    Logger() = default;
    void Log(const str& message, LogLevel = LogLevel::Info, LogCategory = LogCategory::General);
};