#pragma once

enum class TitleType {
    BARONY,
    COUNTY,
    DUCHY,
    KINGDOM,
    EMPIRE,
    COUNT,
};

const std::vector<const char*> TitleTypeLabels = { "Barony", "County", "Duchy", "Kingdom", "Empire" };

inline TitleType GetTitleTypeByName(std::string name) {
    if(name.starts_with("b_")) return TitleType::BARONY;
    if(name.starts_with("c_")) return TitleType::COUNTY;
    if(name.starts_with("d_")) return TitleType::DUCHY;
    if(name.starts_with("k_")) return TitleType::KINGDOM;
    if(name.starts_with("e_")) return TitleType::EMPIRE;
    throw std::runtime_error("error: invalid title name.");
}