#include "Menu.hpp"
#include "app/App.hpp"
// #include "util/Ptr.hpp"

Menu::Menu(App& app, const std::string& name)
: m_App(app), m_Name(name) {}

App& Menu::GetApp() const {
    return m_App;
}

std::string Menu::GetName() const {
    return m_Name;
}

void Menu::Update(sf::Time delta) {}

void Menu::Event(const sf::Event& event) {
    if (const auto* resize = event.getIf<sf::Event::Resized>()) {
		sf::View defaultView = m_App.GetWindow().getDefaultView();
        defaultView.setSize({
            static_cast<float>(resize->size.x),
            static_cast<float>(resize->size.y)
        });
		m_App.GetWindow().setView(defaultView);
    }
}

void Menu::Render() {}