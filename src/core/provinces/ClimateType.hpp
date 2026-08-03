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