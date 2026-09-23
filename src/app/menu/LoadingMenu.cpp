#include "LoadingMenu.hpp"
#include "app/App.hpp"

LoadingMenu::LoadingMenu(App& app, std::function<void()> completeCallback, std::function<void(const std::string&)> errorCallback) :
    Menu(app, "Loading"),
    m_State(static_cast<LoadingState>(0)),
    m_Cancelled(false),
    m_LoadingError(""),
    m_Thread(nullptr),
    m_CompleteCallback(completeCallback), m_ErrorCallback(errorCallback)
{
}

LoadingMenu::~LoadingMenu() {
    if (m_Thread && m_Thread->joinable()) {
        m_Cancelled.store(true);
        m_Thread->join();
	}
}

void LoadingMenu::Update(sf::Time delta) {

}

void LoadingMenu::Event(const sf::Event& event) {
    Menu::Event(event);
}

void LoadingMenu::Render() {
    LoadingState currentState = m_State.load();
    if (currentState == LoadingState::FINISHED) {
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
    ImGui::ProgressBar(((float) (currentState))/LoadingStateLabels.size(), ImVec2(0.0f, 0.0f), LoadingStateLabels.at(currentState).c_str());
    ImGui::PopStyleColor();
    
    ImGui::End();
}

void LoadingMenu::SetState(LoadingState state) {
    m_State.store(state);
}

void LoadingMenu::Start(UniquePtr<Mod> mod) {
    m_Cancelled.store(false);
    m_Thread = MakeUnique<std::thread>([this, mod = std::move(mod)]() mutable {
        mod->Load(
            [this]() { m_State.store(LoadingState::FINISHED); },
            [this](LoadingState state) { m_State.store(state); },
            [this](const std::string& error) { m_LoadingError = error; m_State.store(LoadingState::FINISHED); },
            &m_Cancelled
        );
        m_App.SetMod(std::move(mod));
    });
}