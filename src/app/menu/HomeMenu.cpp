#include "HomeMenu.hpp"
#include "imgui/imgui.hpp"
#include "nfd/nfd.h"

HomeMenu::HomeMenu(App* app)
: Menu(app, "Home") {}

void HomeMenu::Update(sf::Time delta) {

}

void HomeMenu::Event(const sf::Event& event) {

}

void HomeMenu::Draw(sf::RenderWindow& window) {
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(Configuration::windowResolution.x - 20, Configuration::windowResolution.y - 20), ImGuiCond_Always);
    ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

    if(ImGui::Button("Open mod")) {
        nfdchar_t *dirPath = NULL;
        nfdresult_t result = NFD_PickFolder(NULL, &dirPath);
            
        if(result == NFD_OKAY) {
            INFO("Selected directory: {}", dirPath);
            free(dirPath);
        }
        else if(result == NFD_CANCEL) {
            INFO("Cancelled directory selection.", "");
        }
        else {
            ERROR("Error while picking directory: {}", NFD_GetError());
        }
    }
    
    ImGui::NewLine();

    ImGui::End();

}