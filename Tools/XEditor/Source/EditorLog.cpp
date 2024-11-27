// Author: Jake Rieger
// Created: 11/11/2024.
//

#include "EditorLog.h"

str LogLevelToString(const LogLevel& level) {
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

str LogCategoryToString(const LogCategory& category) {
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

str LogEntry::ToString() const {
    std::stringstream ss;
    ss << "["
       << "00:00:00"
       << "] ";
    ss << "|" << LogLevelToString(Level) << "| ";
    ss << "Xen:" << LogCategoryToString(Category) << " ";
    ss << Message << " ";
    return ss.str();
}

void Logger::Log(const str& message, LogLevel, LogCategory) {
    Entries.emplace_back(message, LogLevel::Info, LogCategory::General, std::time(nullptr));
}