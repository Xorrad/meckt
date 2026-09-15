#pragma once

struct EditorSetup {
    Opt<int> selectedProvinceId;
    Opt<std::string> selectedTitleName;
    Opt<MapMode> mapMode;
    Opt<std::string> scriptFilePath;
};