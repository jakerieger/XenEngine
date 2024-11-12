// Author: Jake Rieger
// Created: 11/11/2024.
//

#pragma once

#include <Types/Types.h>
#include <Types/IO.h>
#include <Types/SmartPtr.h>
#define INC_VECTOR
#define INC_DICTIONARY
#define INC_QUEUE
#define INC_MUTEX
#include <Types/STL.h>

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

static str LogLevelToString(const LogLevel& level) {
    switch (level) {
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return "Warning";
        case LogLevel::Error:
            return "Error";
        case LogLevel::Panic:
            return "Panic";
        case LogLevel::Debug:
            return "Debug";
    }

    return "";
}

static str LogCategoryToString(const LogCategory& category) {
    switch (category) {
        case LogCategory::Graphics:
            return "Graphics";
        case LogCategory::Editor:
            return "Editor";
        case LogCategory::Audio:
            return "Audio";
        case LogCategory::Physics:
            return "Physics";
        case LogCategory::Input:
            return "Input";
        case LogCategory::General:
            return "General";
    }

    return "";
}

struct LogEntry {
    str Message;
    LogLevel Level;
    LogCategory Category;
    std::time_t Timestamp;

    [[nodiscard]] str ToString() const {
        std::stringstream ss;
        ss << "["
           << "00:00:00"
           << "] ";
        ss << "|" << LogLevelToString(Level) << "| ";
        ss << "Xen:" << LogCategoryToString(Category) << " ";
        ss << Message << " ";
        return ss.str();
    }
};

class Logger {
public:
    Vector<LogEntry> Entries;

    Logger() = default;

    void Log(const str& message, LogLevel = LogLevel::Info, LogCategory = LogCategory::General) {
        Entries.emplace_back(message, LogLevel::Info, LogCategory::General, std::time(nullptr));
    }
};