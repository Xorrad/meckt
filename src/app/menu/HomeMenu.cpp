#include "HomeMenu.hpp"
#include "EditorMenu.hpp"
#include "NewModMenu.hpp"
#include "app/App.hpp"

#include <nfd.h>

bool HomeMenu::s_PromptUpdate = true;

HomeMenu::HomeMenu(App& app)
: Menu(app, "Home"), m_LoadingError("") {}

HomeMenu::HomeMenu(App& app, std::string loadingError)
: Menu(app, "Home"), m_LoadingError(loadingError) {}

void HomeMenu::Update(sf::Time delta) {

}

void HomeMenu::Event(const sf::Event& event) {
    Menu::Event(event);
}

void HomeMenu::Render() {
    std::string openedModDir("");

    float margin = 40.0f;
    float spacing = 2.5f;

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(m_App.GetWindow().getSize().x - 20, m_App.GetWindow().getSize().y - 20), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(margin, 0.0f));
    ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
    ImGui::PopStyleVar();

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 windowPos = ImGui::GetWindowPos();
    float marginTop = 0.08f * windowSize.y;

    ImGui::SetCursorPos(ImVec2(margin, marginTop));

    // Title.
    ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_LARGE);
    ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.00f), "meckt");
    ImGui::PopFont();

    // Version and credits.
    ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_MEDIUM);
    ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), fmt::format("v{} - {}", Configuration::buildVersion, Configuration::buildCredits).c_str());
    ImGui::PopFont();

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, spacing));

    // Start section (new project, open directory...).
    ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_MEDIUM);
    ImGui::Text("Start");
    ImGui::PopFont();

    ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_SMALL);

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::BeginDisabled();
    if (ImGui::TextButton("📝  New Mod...")) {
        m_App.OpenMenu(MakeUnique<NewModMenu>(m_App));
    }
    if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Create a mod from scratch.");
        ImGui::SetTooltip("Disabled for now due to a lack of up-to-date TC templates.");
    }
    ImGui::EndDisabled();

    ImGui::Dummy(ImVec2(0.0f, spacing));
    if (ImGui::TextButton("📁  Open Folder...")) {
        nfdchar_t *dirPath = NULL;
        nfdresult_t result = NFD_PickFolder(NULL, &dirPath);
            
        if(result == NFD_OKAY) {
            openedModDir = std::string(dirPath);
        }
        else if(result != NFD_CANCEL) {
            LOG_ERROR("Failed to open mod at {}", NFD_GetError());
            m_LoadingError = fmt::format("Failed to open mod ({})", NFD_GetError());
        }
        free(dirPath);
    }
    if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Open an existing mod.");
    }

    ImGui::PopFont();

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, spacing));

    // Recent section (last opened mod directories).
    ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_MEDIUM);
    ImGui::Text("Recent");
    ImGui::PopFont();

    ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_SMALL);
    int i = 0;
    for (auto dir : Configuration::recentMods) {
        if (i == 5) break;
        if (dir.ends_with("/")) dir.pop_back();
        std::filesystem::path path(dir);

        ImGui::Dummy(ImVec2(0.0f, spacing));
        if (ImGui::TextButton(path.filename().string().c_str())) {
            openedModDir = dir;
            break;
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(5.0f, 0.0f));
        ImGui::SameLine();
        ImGui::Text(std::filesystem::absolute(path).string().c_str());
        i++;
    }
    ImGui::PopFont();

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // Other section.
    ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_SMALL);
    if (ImGui::TextButton("🐛 Report an issue")) {
        std::string command;
#ifdef _WIN32
        command = "start " + Configuration::githubURL + "/issues";
#else
        command = "xdg-open " + Configuration::githubURL + "/issues" + "&>/dev/null";
#endif
        if(std::system(command.c_str())) {}
    }
    ImGui::Dummy(ImVec2(0.0f, spacing));
    if (ImGui::TextButton("❌ Exit")) {
        m_App.GetWindow().close();
    }
    ImGui::PopFont();
    
    ImGui::NewLine();
    ImGui::End();

    // Determine which modal should be displayed.
    if (s_PromptUpdate && m_App.GetUpdateDetails().shouldUpdate) {
        ImGui::OpenPopup("Update");
        this->RenderUpdateModal();
    }
    else if (!m_LoadingError.empty()) {
        ImGui::OpenPopup("Error");
        this->RenderErrorModal();
    }

    // Open the mod at the end to avoid crashes because of ImGui.
    if (!openedModDir.empty()) {
        UniquePtr<Mod> mod = MakeUnique<Mod>(openedModDir);
        if(mod->HasMap()) {
            try {
                m_App.OpenMod(std::move(mod));
                LOG_INFO("Opened mod at {}", openedModDir);
            }
            catch (std::exception& e) {
                LOG_INFO("Failed to load mod at {}\n{}", openedModDir, e.what());
                m_LoadingError = "Failed to load mod.\nOpen an issue on GitHub or contact the developper\non Discord if the issue persists.";
            }
        }
        else {
            LOG_INFO("Opened mod missing 'map_data/provinces.png' at {}", openedModDir);
            m_LoadingError = "This mod does not have a provinces image.";
        }
        openedModDir.clear();
    }
}

void HomeMenu::RenderErrorModal() {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), m_LoadingError.c_str());
        ImGui::Separator();

        if(ImGui::Button("Open logs", ImVec2(120, 0))) {
            File::OpenFile("logs");
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Close", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            m_LoadingError = "";
        }
        ImGui::EndPopup();
    }
}

void HomeMenu::RenderUpdateModal() {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400.f, 0.f));
    if(ImGui::BeginPopupModal("Update", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("A newer version is available!");
        ImGui::Text(fmt::format("Current: v{} -> Latest: v{}", Configuration::buildVersion, m_App.GetUpdateDetails().lastBuildVersion).c_str());

        bool hasError = !m_App.GetUpdateDetails().error.empty();
        if (hasError) {
            ImGui::PushTextWrapPos();
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), m_App.GetUpdateDetails().error.c_str());
            ImGui::PopTextWrapPos();
        }
        ImGui::Separator();

        if(ImGui::Button("Open GitHub")) {
            std::string command;
#ifdef _WIN32
            command = "start " + m_App.GetUpdateDetails().lastBuildURL;
#else
            command = "xdg-open " + m_App.GetUpdateDetails().lastBuildURL + "&>/dev/null";
#endif
            if(std::system(command.c_str())) {}
        }

        ImGui::SameLine();
        if(ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            s_PromptUpdate = false;
        }
        ImGui::EndPopup();
    }
}