#pragma once

#include <fstream>
#include <filesystem>
#include <fmt/core.h>

#define LOGS_FILE "logs/logs.txt"
#define CRASH_FILE "logs/crash.txt"

#define TEST(n) std::cout << n << std::endl

namespace Logger
{
    std::string GetTime();

    template <typename ...Args>
    void LogMessage(const std::string& filePath, const std::string& originFile, int line, const std::string& func, const std::string& type, const std::string& fmt, Args... args)
    {
        std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
    
        std::string message = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
        std::ofstream file(filePath, std::ios_base::app);

        if(!file.is_open())
            return;

        file << GetTime() << "[" << originFile << ":" << line << "][" << func << "] " << type << ": " << message << std::endl;
        
        file.close();
    }

    void ClearLogs();
}

#define ERROR(fmt, ...)   Logger::LogMessage(LOGS_FILE, __FILE__, __LINE__, __func__, "ERROR", fmt, ##__VA_ARGS__)
#define WARNING(fmt, ...) Logger::LogMessage(LOGS_FILE, __FILE__, __LINE__, __func__, "WARNING", fmt, ##__VA_ARGS__)
#define INFO(fmt, ...)    Logger::LogMessage(LOGS_FILE, __FILE__, __LINE__, __func__, "INFO", fmt, ##__VA_ARGS__)

#define FATAL(fmt, ...) \
    Logger::LogMessage(LOGS_FILE,     __FILE__, __LINE__, __func__, "FATAL", fmt, ## __VA_ARGS__); \
    Logger::LogMessage(LOGS_FILE,     __FILE__, __LINE__, __func__, "FATAL", "-- FATAL ERROR --"); \
    exit(EXIT_FAILURE);
                        
