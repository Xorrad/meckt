#pragma once

class Mod {
public:
    Mod(const std::string& dir);

    std::string GetDir() const;
    bool HasMap() const;

private:
    std::string m_Dir;
};