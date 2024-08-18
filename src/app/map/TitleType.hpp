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
const std::vector<const char*> TitleTypePrefixes = { "b", "c", "d", "k", "e" };

inline TitleType GetTitleTypeByName(const std::string& name) {
    for(int i = 0; i < (int) TitleType::COUNT; i++) {
        std::string prefix = std::string(TitleTypePrefixes[i]) + "_";
        if(name.starts_with(prefix))
            return (TitleType) i;
    }
    throw std::runtime_error("error: invalid title name.");
}

inline std::string GetTitlePrefixByType(TitleType type) {
    return TitleTypePrefixes[(int) type];
}

inline bool IsValidTitleName(const std::string& name, TitleType type) {
    try {
        GetTitleTypeByName(name);
        return true;
    }
    catch (const std::runtime_error& e){}
    return true;
}