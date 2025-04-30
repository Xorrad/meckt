#include "HomeMenu.hpp"
#include "EditorMenu.hpp"
#include "app/App.hpp"
#include "ImGuiStyle.hpp"
#include "imgui/imgui.hpp"
#include "nfd/nfd.h"

HomeMenu::HomeMenu(App* app)
: Menu(app, "Home"), m_LoadingError("") {}

HomeMenu::HomeMenu(App* app, std::string loadingError)
: Menu(app, "Home"), m_LoadingError(loadingError) {}

void HomeMenu::Update(sf::Time delta) {

}

void HomeMenu::Event(const sf::Event& event) {

}

void HomeMenu::Render() {
    static bool showUpdateModal = true;

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(m_App->GetWindow().getSize().x - 20, m_App->GetWindow().getSize().y - 20), ImGuiCond_Always);
    ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 windowPos = ImGui::GetWindowPos();

    float spacing = 20.0f;
    ImVec2 buttonSize = ImVec2(200.0f, 50.0f);

    const sf::Texture& logoTexture = Configuration::textures.Get(Textures::LOGO);

    float totalHeight = logoTexture.getSize().y + buttonSize.y * 2 + spacing * 2;
    float startY = (windowSize.y - totalHeight) * 0.4f + windowPos.y;
    float centerX = windowSize.x * 0.5f + windowPos.x;

    ImGui::SetCursorPos(ImVec2(centerX - logoTexture.getSize().x*0.5f, startY));
    ImGui::Image(logoTexture);

    ImGui::SetCursorPos(ImVec2(centerX - buttonSize.x*0.5f, startY + logoTexture.getSize().y + spacing));
    if(ImGui::Button("Open mod", buttonSize)) {
        nfdchar_t *dirPath = NULL;
        nfdresult_t result = NFD_PickFolder(NULL, &dirPath);
            
        if(result == NFD_OKAY) {
            SharedPtr<Mod> mod = MakeShared<Mod>(std::string(dirPath));
            if(mod->HasMap()) {
                try {
                    m_App->OpenMod(mod);
                    LOG_INFO("Opened mod at {}", dirPath);

                    // End the current window to avoid crash.
                    ImGui::End();
                    return;
                }
                catch (std::exception& e) {
                    LOG_INFO("Failed to load mod at {}\n{}", dirPath, e.what());
                    m_LoadingError = "Failed to load mod.\nOpen an issue on GitHub or contact the developper\non Discord if the issue persists.";
                }
            }
            else {
                LOG_INFO("Opened mod missing 'map_data/provinces.png' at {}", dirPath);
                m_LoadingError = "This mod does not have a provinces image.";
            }
        }
        else if(result != NFD_CANCEL) {
            LOG_ERROR("Failed to open mod at {}", NFD_GetError());
            m_LoadingError = fmt::format("Failed to open mod ({})", NFD_GetError());
        }
        free(dirPath);
    }
    if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Select a mod directory (with descriptor file)");
    }

    ImGui::SetCursorPos(ImVec2(centerX - buttonSize.x*0.5f, startY + logoTexture.getSize().y + spacing * 2 + buttonSize.y));
    if(ImGui::Button("Exit", buttonSize)) {
        m_App->GetWindow().close();
    }

    ImVec2 textSize = ImGui::CalcTextSize(Configuration::buildVersion.c_str());
    ImGui::SetCursorPos(ImVec2(5.0f, windowSize.y - textSize.y - 5.0f));
    ImGui::Text(Configuration::buildVersion.c_str());

    textSize = ImGui::CalcTextSize(Configuration::buildCredits.c_str());
    ImVec2 creditsPos = ImVec2(windowSize.x - textSize.x - 5.0f, windowSize.y - textSize.y - 5.0f);
    ImGui::SetCursorPos(creditsPos);
    creditsPos = ImGui::GetCursorScreenPos();
    ImGui::Text(Configuration::buildCredits.c_str());
    if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        std::string command;
#ifdef _WIN32
        command = "start " + Configuration::githubURL;
#else
        command = "xdg-open " + Configuration::githubURL + "&>/dev/null";
#endif
        if(std::system(command.c_str())) {}
    }
    if(ImGui::IsItemHovered()) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddLine(
            ImVec2(creditsPos.x - 2.0f, creditsPos.y + textSize.y + 0.5f),
            ImVec2(creditsPos.x + textSize.x + 2.0f, creditsPos.y + textSize.y + 0.5f),
            ImGui::GetColorU32(ImGuiCol_Text),
            1.0f
        );
    }

    ImGui::End();
    
    if (showUpdateModal && m_App->GetUpdateDetails().shouldUpdate)
        ImGui::OpenPopup("Update");
    else if (!m_LoadingError.empty())
        ImGui::OpenPopup("Error");

    // UPDATE : modal begin
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400.f, 0.f));
    if(ImGui::BeginPopupModal("Update", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("A newer version is available!");
        ImGui::Text(fmt::format("Current: v{} -> Latest: v{}", Configuration::buildVersion, m_App->GetUpdateDetails().lastBuildVersion).c_str());

        bool hasError = !m_App->GetUpdateDetails().error.empty();
        if (hasError) {
            ImGui::PushTextWrapPos();
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), m_App->GetUpdateDetails().error.c_str());
            ImGui::PopTextWrapPos();
        }
        ImGui::Separator();

        ImGui::SetItemDefaultFocus();
        if (hasError) ImGui::BeginDisabled();
        if(ImGui::Button("Download", ImVec2(120, 0))) {
            m_App->GetUpdateDetails().error = Update::Update(m_App->GetUpdateDetails());

            // Only restart if the download has been successful.
            if (m_App->GetUpdateDetails().error.empty())
                m_App->GetWindow().close();
        }
        if (hasError) ImGui::EndDisabled();

        ImGui::SameLine();
        if(ImGui::Button("Close", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            showUpdateModal = false;
        }
        ImGui::EndPopup();
    }
    // UPDATE: modal end
    
    // ERROR : modal begin
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
    // ERROR: modal end
}