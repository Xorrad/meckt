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