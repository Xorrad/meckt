#include "Logger.hpp"

std::string GetLogsFileName(const std::string& name) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", ltm);
    std::string date = std::string(buffer);

    std::string originalPath = "logs/" + name + "-" + date;
    std::string path = originalPath;
    int i = 1;
    while (std::filesystem::exists(path + ".txt")) {
        path = originalPath + "-" + std::to_string(i);
        i++;
    }
    return path + ".txt";
}

const std::string LOGS_FILE = GetLogsFileName("logs");
const std::string CRASH_FILE = GetLogsFileName("crash");

Logger::Message::Message() : Message(time(0), "", 0, "", MessageType::INFO, "") {}

Logger::Message::Message(std::string file, size_t line, std::string function, MessageType type, std::string text) :
    Message(time(0), file, line, function, type, text)
{}

Logger::Message::Message(time_t time, std::string file, size_t line, std::string function, MessageType type, std::string text) :
    m_Time(time),
    m_File(file),
    m_Line(line),
    m_Function(function),
    m_Type(type),
    m_Text(text)
{}

time_t Logger::Message::GetTime() const {
    return m_Time;
}

std::string Logger::Message::GetFile() const {
    return m_File;
}

size_t Logger::Message::GetLine() const {
    return m_Line;
}

std::string Logger::Message::GetFunction() const {
    return m_Function;
}

Logger::MessageType Logger::Message::GetType() const {
    return m_Type;
}

std::string Logger::Message::GetText() const {
    return m_Text;
}

std::string Logger::Message::GetTimeHMS() const {
    tm time;
    char buf[80];
    time = *localtime(&m_Time);
    strftime(buf, sizeof(buf), "%X", &time);
    return std::string(buf);
}

std::string Logger::Message::ToString() const {
    return fmt::format("[{}][{}:{}][{}] {}: {}",
        this->GetTimeHMS(),
        m_File,
        m_Line,
        m_Function,
        MessageTypeLabels[(int) m_Type],
        m_Text
    );
}

sf::Color Logger::Message::GetColor() const {
    switch(m_Type) {
        case FATAL: return sf::Color(255, 0, 0);
        case ERROR_: return sf::Color(255, 0, 0);
        case WARNING: return sf::Color(255, 130, 0);
        case INFO: return sf::Color(0, 140, 255);
        default: return sf::Color::White;
    }
}

Logger::Logger::Logger(std::string filePath) {
    std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
    m_OutputFile.open(filePath, std::ios::binary);
}

Logger::Logger::~Logger() {
    this->Close();
}

std::deque<SharedPtr<Logger::Message>>& Logger::Logger::GetMessages() {
    return m_Messages;
}

void Logger::Logger::PushMessage(SharedPtr<Message> message) {
    m_Messages.push_back(message);
}

void Logger::Logger::RemoveMessage(SharedPtr<Message> message) {
    m_Messages.erase(std::remove(m_Messages.begin(), m_Messages.end(), message));
}

void Logger::Logger::PrintMessage(SharedPtr<Message> message) {
    if(!m_OutputFile.is_open())
        return;
    m_OutputFile << message->ToString() << std::endl;
}

void Logger::Logger::Clear() {
    m_OutputFile.close();
    std::filesystem::remove(LOGS_FILE);
    std::filesystem::remove(CRASH_FILE);
    m_OutputFile.open(LOGS_FILE, std::ios::binary);
    m_Messages.clear();
}

void Logger::Logger::Close() {
    if(m_OutputFile)
        m_OutputFile.close();
}

UniquePtr<Logger::Logger> Logger::Logger::s_Instance = MakeUnique<Logger>(LOGS_FILE);

UniquePtr<Logger::Logger>& Logger::Get() {
    return Logger::Logger::s_Instance;
}