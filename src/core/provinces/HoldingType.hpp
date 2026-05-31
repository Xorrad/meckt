#pragma once

class HoldingType {
public:
    HoldingType();
    HoldingType(const std::string& name);

    std::string GetName() const;
    void SetName(const std::string& name);

private:
    std::string m_Name;
};