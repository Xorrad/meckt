#pragma once

using Directory = std::string_view;

namespace Paths {
    // Common directory paths.
    static constexpr Directory COMMON_LANDED_TITLES = "common/landed_titles/";

    // History directory paths.
    static constexpr Directory HISTORY_TITLES = "history/titles/";

    // Localization directory paths.
    // TODO: rework paths system for different localization languages.
    static constexpr Directory LOCALIZATION_ENGLISH = "localization/english/";
    static constexpr Directory LOCALIZATION_REPLACE_ENGLISH = "localization/replace/english/";
}