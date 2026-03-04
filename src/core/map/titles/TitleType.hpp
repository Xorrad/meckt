#pragma once

enum class TitleType {
    BARONY,
    COUNTY,
    DUCHY,
    KINGDOM,
    EMPIRE,
    HEGEMONY,
    COUNT,
};

const std::vector<const char*> TitleTypeLabels = { "Barony", "County", "Duchy", "Kingdom", "Empire", "Hegemony" };
const std::vector<const char*> TitleTypePrefixes = { "b", "c", "d", "k", "e", "h" };

inline TitleType GetTitleTypeByName(const std::string& name) {
    if (name.size() > 2 && name[1] == '_' && name[2] != ' ') {
        char ch = name[0];
        for(int i = 0; i < static_cast<int>(TitleType::COUNT); i++) {
            if (TitleTypePrefixes[i][0] == ch)
                return static_cast<TitleType>(i);
        }
    }
    throw std::invalid_argument("GetTitleTypeByName: invalid title name");
}

inline std::string GetTitlePrefixByType(TitleType type) {
    return TitleTypePrefixes[static_cast<int>(type)];
}

inline bool IsValidTitleName(const std::string& name, TitleType type) {
    if (name.size() < 3)
        return false;
    if (name[1] != '_')
        return false;
    if (name[2] == ' ')
        return false;
    if (TitleTypePrefixes.size() <= static_cast<int>(type))
        return false;
    return TitleTypePrefixes[static_cast<int>(type)][0] == name[0];
}