#include  "NameInputs.hpp"

bool Components::TitleNameInput(std::string_view label, const std::string& currentName, TitleType type, std::function<void(std::string)> onChange) {
    bool valueChanged = false;

    ImGui::PushID(ImHashStr(fmt::format("##title-name-input-{}", label).c_str()));

    std::string newName = currentName;
    if (ImGui::InputTextCommitOnEnter(
        "name",
        &newName,
        ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter,
        Components::Filters::TitleName
    )) {
        if (!newName.starts_with(TitleTypePrefixes[static_cast<int>(type)]))
            newName = fmt::format("{}_{}", TitleTypePrefixes[static_cast<int>(type)], newName);
        if (!newName.empty() && IsValidTitleName(newName, type)) {
            onChange(newName);
            valueChanged = true;
        }
    }

    ImGui::PopID();

    return valueChanged;
}

bool Components::ProvinceNameInput(std::string_view label, const std::string& currentName, std::function<void(std::string)> onChange) {
    bool valueChanged = false;

    ImGui::PushID(ImHashStr(fmt::format("##province-name-input-{}", label).c_str()));

    std::string newName = currentName;
    if (ImGui::InputTextCommitOnEnter(
        "name",
        &newName,
        ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter,
        Components::Filters::ProvinceName
    )) {
        if (!newName.empty()) {
            onChange(newName);
            valueChanged = true;
        }
    }

    ImGui::PopID();

    return valueChanged;
}

bool Components::RegionNameInput(std::string_view label, const std::string& currentName, std::function<void(std::string)> onChange) {
    bool valueChanged = false;

    ImGui::PushID(ImHashStr(fmt::format("##region-name-input-{}", label).c_str()));

    std::string newName = currentName;
    if (ImGui::InputTextCommitOnEnter(
        "name",
        &newName,
        ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter,
        Components::Filters::RegionName
    )) {
        if (!newName.empty()) {
            onChange(newName);
            valueChanged = true;
        }
    }

    ImGui::PopID();

    return valueChanged;
}