#include "Logger.hpp"

std::string Logger::GetTime()
{
    time_t now = time(0);
    tm time;
    char buf[80];
    time = *localtime(&now);
    strftime(buf, sizeof(buf), "[%X]", &time);
    return std::string(buf);
}

void Logger::ClearLogs()
{
    std::filesystem::remove(LOGS_FILE);
    std::filesystem::remove(CRASH_FILE);
}