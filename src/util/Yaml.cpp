#include "Yaml.hpp"

std::map<std::string, std::string> Yaml::ParseFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string content = File::ReadString(file);
    file.close();
    return Parse(content);
}

std::map<std::string, std::string> Yaml::ParseFile(std::ifstream& file) {
    std::string content = File::ReadString(file);
    return Parse(content);
}

std::map<std::string, std::string> Yaml::Parse(const std::string& content) {
    std::map<std::string, std::string> values;

    uint i = 0;
    std::string key = "";
    std::string buffer = "";

    auto SkipTo = [&](char stop, char second = '\n') {
        while(i < content.size() && content[i] != stop && content[i] != second)
            i++;
    };
    
    auto CaptureBuffer = [&](char stop) {
        while(i < content.size() && content[i] != stop) {
            buffer += content[i];
            i++;
        }
    };

    for(i = 0; i < content.size(); i++) {
        // Ignore special characters.
        if(content[i] == '\n' || content[i] == '\t' || content[i] == '\r' || content[i] == ' ')
            continue;
        // Skip till next line on comments.
        if(content[i] == '#') {
            SkipTo('\n');
            continue;
        }
        // Parse value after key.
        if(content[i] == ':') {
            // Reset buffer and skip to first quotes.
            key = buffer;
            buffer = "";
            SkipTo('"', '\n');

            // If there is a new line between the key and the next quote
            // then the value will be an empty string.
            if(i < content.size() && content[i++] == '"') {
                // Read to buffer until second quotes and push key-value pair.
                CaptureBuffer('"');
            }

            values[key] = buffer;
            buffer = "";
            continue;
        }
        buffer += content[i];
    }

    return values;
}