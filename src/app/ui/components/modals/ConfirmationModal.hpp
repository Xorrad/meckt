#pragma once

namespace Components {
    /**
     * @brief Renders a confirmation modal that invokes a function if the user confirms.
     * @param id The id of the modal.
     * @param text The text to display to the user.
     * @param action The function to invoke if the user confirms.
     */
    template<typename F>
    void ConfirmationModal(std::string_view id, std::string_view text, F&& action) {
        if (ImGui::BeginPopupModal(id.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextWrapped("%s", text.data());
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone!");
            ImGui::Separator();

            if (ImGui::Button("OK")) {
                action();
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
    }
}