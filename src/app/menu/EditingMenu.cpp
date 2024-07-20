#include "EditingMenu.hpp"
#include "HomeMenu.hpp"
#include "app/App.hpp"
#include "imgui/imgui.hpp"

EditingMenu::EditingMenu(App* app)
: Menu(app, "Editor") {}

void EditingMenu::Update(sf::Time delta) {

}

void EditingMenu::Event(const sf::Event& event) {

}

void EditingMenu::Draw(sf::RenderWindow& window) {

    if(ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Close")) {
                m_App->OpenMenu(MakeUnique<HomeMenu>(m_App));
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}