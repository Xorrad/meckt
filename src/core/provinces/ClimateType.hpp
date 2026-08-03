#pragma once

enum class ClimateType {
    NONE,
    MILD_WINTER,
    NORMAL_WINTER,
    SEVERE_WINTER,
    COUNT
};

const std::unordered_map<ClimateType, const char*> ClimateTypeLabels = {
    { ClimateType::NONE, "None" },
    { ClimateType::MILD_WINTER, "Mild Winter" },
    { ClimateType::NORMAL_WINTER, "Normal Winter" },
    { ClimateType::SEVERE_WINTER, "Severe Winter" },
    { ClimateType::COUNT, "******" }
};

const std::unordered_map<ClimateType, sf::Color> ClimateTypeColors = {
    { ClimateType::NONE, sf::Color(25, 25, 25) },
    { ClimateType::MILD_WINTER, sf::Color(100, 100, 100) },
    { ClimateType::NORMAL_WINTER, sf::Color(175, 175, 175) },
    { ClimateType::SEVERE_WINTER, sf::Color(255, 255, 255) },
    { ClimateType::COUNT, sf::Color(0, 0, 0) }
};