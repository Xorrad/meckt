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

/**
 * @brief Gets the title type by its name.
 * @param name The name of the title.
 * @return The title type.
 */
inline TitleType GetTitleTypeByName(const std::string& name) {
    if (name.size() > 2 && name[1] == '_' && name[2] != ' ') {
        for(int i = 0; i < static_cast<int>(TitleType::COUNT); i++) {
            if (TitleTypePrefixes[i][0] == name[0])
                return static_cast<TitleType>(i);
        }
    }
    throw std::invalid_argument("GetTitleTypeByName: invalid title name");
}

/**
 * @brief Gets the prefix for a title type.
 * @param type The title type.
 * @return The prefix for the title type.
 */
inline std::string GetTitlePrefixByType(TitleType type) {
    return TitleTypePrefixes[static_cast<int>(type)];
}

/**
 * @brief Checks if a title name is valid for a specific title type.
 * @param name The title name to check.
 * @param type The title type.
 * @return True if the title name is valid for the specified title type, false otherwise.
 */
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

/**
 * @brief Checks if a title name is valid.
 * @param name The title name to check.
 * @return True if the title name is valid, false otherwise.
 */
inline bool IsValidTitleName(const std::string& name) {
    if (name.size() < 3)
        return false;
    if (name[1] != '_')
        return false;
    if (name[2] == ' ')
        return false;
    for(int i = 0; i < static_cast<int>(TitleType::COUNT); i++) {
        if (TitleTypePrefixes[i][0] == name[0])
            return true;
    }
    return false;
}