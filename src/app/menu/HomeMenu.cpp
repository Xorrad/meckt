#include "HomeMenu.hpp"
#include "EditingMenu.hpp"
#include "app/App.hpp"
#include "imgui/imgui.hpp"
#include "nfd/nfd.h"

HomeMenu::HomeMenu(App* app)
: Menu(app, "Home") {}

void HomeMenu::Update(sf::Time delta) {

}

void HomeMenu::Event(const sf::Event& event) {

}

void HomeMenu::Draw() {
    static std::string error = "";

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(Configuration::windowResolution.x - 20, Configuration::windowResolution.y - 20), ImGuiCond_Always);
    ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

    if(ImGui::Button("Open mod")) {
        nfdchar_t *dirPath = NULL;
        nfdresult_t result = NFD_PickFolder(NULL, &dirPath);
            
        if(result == NFD_OKAY) {
            UniquePtr<Mod> mod = MakeUnique<Mod>(std::string(dirPath));
            if(mod->HasMap()) {
                m_App->OpenMenu(MakeUnique<EditingMenu>(m_App, std::move(mod)));
                INFO("Opened mod at {}", dirPath);
            }
            else {
                INFO("Opened mod without custom map at {}", dirPath);
                error = fmt::format("This mod does not have a custom map.");
            }
            free(dirPath);
        }
        else if(result != NFD_CANCEL) {
            ERROR("Failed to open mod at {}", NFD_GetError());
            error = fmt::format("Failed to open mod ({})", NFD_GetError());
        }
    }
    if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Select a mod directory (with descriptor file)");
    }
    
    if(!error.empty()) {
        ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), error.c_str());
    }

    ImGui::End();

}