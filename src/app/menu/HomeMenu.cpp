#include "HomeMenu.hpp"
#include "imgui/imgui.hpp"

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

    }
    
    ImGui::NewLine();

    ImGui::End();

}