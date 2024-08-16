#include "Tab.hpp"
#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/menu/EditorMenu.hpp"

Tab::Tab(std::string name, Tabs type, EditorMenu* menu, bool visible)
: m_Name(name), m_Type(type), m_Menu(menu), m_Visible(visible) {}

std::string Tab::GetName() const {
    return m_Name;
}

Tabs Tab::GetType() const {
    return m_Type;
}

bool Tab::IsVisible() const {
    return m_Visible;
}

bool& Tab::IsVisible() {
    return m_Visible;
}
    
void Tab::SetVisible(bool visible) {
    m_Visible = visible;
}

void Tab::Update(sf::Time delta) {}

void Tab::Event(const sf::Event& event) {}

void Tab::Render() {}

App* Tab::GetApp() {
    return m_Menu->GetApp();
}

SharedPtr<Mod> Tab::GetMod() {
    return m_Menu->GetApp()->GetMod();
}