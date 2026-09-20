#include  "NameInputs.hpp"

bool Components::TitleNameInput(std::string_view label, const std::string& currentName, TitleType type, std::function<void(std::string)> onChange) {
    bool valueChanged = false;

    ImGui::PushID(ImHashStr(fmt::format("##title-name-input-{}", label).c_str()));

    std::string newName = currentName;
    if (ImGui::InputTextCommitOnEnter(
        "name",
        &newName,
        ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter,
        Components::Filters::TitleName,
        nullptr,
        [type](const std::string& name) { return IsValidTitleName(name, type); }
    )) {
        if (IsValidTitleName(newName, type)) {
            onChange(newName);
            valueChanged = true;
        }
    }

    ImGui::PopID();

    return valueChanged;
}