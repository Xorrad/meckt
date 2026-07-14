#include "HoldingType.hpp"

HoldingType::HoldingType() : m_Name("") {}

HoldingType::HoldingType(const std::string& name) :
    m_Name(name)
{}

//////////////////////////////////////////////////////

std::string HoldingType::GetName() const {
    return m_Name;
}

//////////////////////////////////////////////////////

void HoldingType::SetName(const std::string& name) {
    m_Name = name;
}