#include "LoadingMenu.hpp"
#include "app/App.hpp"

#include "ImGuiStyle.hpp"
#include <imgui/imgui.hpp>

LoadingMenu::LoadingMenu(App& app, std::function<void()> completeCallback, std::function<void(const std::string&)> errorCallback) :
    Menu(app, "Loading"),
    m_State((LoadingState) 0),
    m_LoadingError(""),
    m_Thread(nullptr),
    m_CompleteCallback(completeCallback), m_ErrorCallback(errorCallback)
{
}

LoadingMenu::~LoadingMenu() {
    if (m_Thread && m_Thread->joinable()) {
        m_Thread->join();
	}
}

void LoadingMenu::Update(sf::Time delta) {

}

void LoadingMenu::Event(const sf::Event& event) {
    Menu::Event(event);
}

void LoadingMenu::Render() {
    if (m_State == LoadingState::FINISHED) {
        if (m_LoadingError.empty()) {
            m_CompleteCallback();
            return;
        }
        m_ErrorCallback(m_LoadingError);
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(m_App.GetWindow().getSize().x - 20, m_App.GetWindow().getSize().y - 20), ImGuiCond_Always);
    ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 progressbarSize = ImVec2(0.7f*windowSize.x, 50.0f);

    float startY = (windowSize.y - progressbarSize.y) * 0.4f + windowPos.y;
    float centerX = windowSize.x * 0.5f + windowPos.x;

    ImGui::SetCursorPos(ImVec2(centerX - progressbarSize.x*0.5f, startY));
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.40f, 0.40f, 0.90f, 0.45f));
    ImGui::ProgressBar(((float) m_State)/LoadingStateLabels.size(), ImVec2(0.0f, 0.0f), LoadingStateLabels.at(m_State).c_str());
    ImGui::PopStyleColor();
    
    ImGui::End();
}

void LoadingMenu::SetState(LoadingState state) {
    m_State = state;
}

void LoadingMenu::Start() {
    m_Thread = MakeUnique<std::thread>([&]() {
        m_App.GetMod().Load(
            [&]() { m_State = LoadingState::FINISHED; },
            [&](LoadingState state) { m_State = state; },
            [&](const std::string& error) { m_LoadingError = error; m_State = LoadingState::FINISHED; }
        );
    });
}