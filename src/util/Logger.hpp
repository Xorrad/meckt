#pragma once

#include <fstream>
#include <filesystem>
#include <fmt/core.h>

#define LOGS_FILE "logs/logs.txt"
#define CRASH_FILE "logs/crash.txt"

#define TEST(n) std::cout << n << std::endl

namespace Logger {
    enum MessageType {
        FATAL,
        ERROR,
        WARNING,
        INFO,
    };
    const std::vector<std::string> MessageTypeLabels = { "FATAL", "ERROR", "WARNING", "INFO" };

    class Message {
    public:
        Message();
        Message(std::string file, uint line, std::string function, MessageType type, std::string text);
        Message(time_t time, std::string file, uint line, std::string function, MessageType type, std::string text);

        time_t GetTime() const;
        std::string GetFile() const;
        uint GetLine() const;
        std::string GetFunction() const;
        MessageType GetType() const;
        std::string GetText() const;
        std::string GetTimeHMS() const;

        std::string ToString() const;
        sf::Color GetColor() const;
    private:
        time_t m_Time;
        std::string m_File;
        uint m_Line;
        std::string m_Function;
        MessageType m_Type;
        std::string m_Text;
    };

    class Logger {
    public:
        Logger(std::string filePath);
        ~Logger();

        std::deque<SharedPtr<Message>>& GetMessages();

        void PushMessage(SharedPtr<Message> message);
        void RemoveMessage(SharedPtr<Message> message);
        void PrintMessage(SharedPtr<Message> message);

        void Clear();
        void Close();

        static UniquePtr<Logger> s_Instance;
    private:
        std::deque<SharedPtr<Message>> m_Messages;
        std::ofstream m_OutputFile;
    };

    UniquePtr<Logger>& Get();

    template <typename ...Args>
    void LogMessage(const std::string& filePath, const std::string& sourceFile, uint line, const std::string& func, const MessageType& type, const std::string& fmt, Args... args) {
        std::string text = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
        SharedPtr<Message> message = MakeShared<Message>(sourceFile, line, func, type, text);
        Get()->PushMessage(message);
        Get()->PrintMessage(message);
    }
}

#define ERROR(fmt, ...)   Logger::LogMessage(LOGS_FILE, __FILE__, __LINE__, __func__, Logger::ERROR, fmt, ##__VA_ARGS__)
#define WARNING(fmt, ...) Logger::LogMessage(LOGS_FILE, __FILE__, __LINE__, __func__, Logger::WARNING, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...)    Logger::LogMessage(LOGS_FILE, __FILE__, __LINE__, __func__, Logger::INFO, fmt, ##__VA_ARGS__)

#define FATAL(fmt, ...) \
    Logger::LogMessage(LOGS_FILE,     __FILE__, __LINE__, __func__, Logger::FATAL, fmt, ## __VA_ARGS__); \
    Logger::LogMessage(LOGS_FILE,     __FILE__, __LINE__, __func__, Logger::FATAL, "-- FATAL ERROR --"); \
    exit(EXIT_FAILURE);
                        
