#include "NewModMenu.hpp"

#include "app/App.hpp"

#include "HomeMenu.hpp"
#include "EditorMenu.hpp"

#include "ImGuiStyle.hpp"
#include "imgui/imgui.hpp"
#include "nfd/nfd.h"
#include <filesystem>

NewModMenu::NewModMenu(App* app) :
    Menu(app, "New Mod"),
    m_ModName("My Mod"),
    m_ModPath(std::filesystem::current_path().string() + "/my_mod"),
    m_TemplateType(TemplateType::DEFAULT),
    m_ProvincesImagePath(""),
    m_HeightmapImagePath(""),
    m_WaterLevel(3.8f)
{}

void NewModMenu::Update(sf::Time delta) {

}

void NewModMenu::Event(const sf::Event& event) {

}

void NewModMenu::Render() {
    float margin = 40.0f;
    float spacing = 2.5f;

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(m_App->GetWindow().getSize().x - 20, m_App->GetWindow().getSize().y - 20), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(margin, 0.0f));
    ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
    ImGui::PopStyleVar();

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 windowPos = ImGui::GetWindowPos();
    float marginTop = 0.08f * windowSize.y;
    float availableWidth = windowSize.x - 2.0f*margin - spacing;

    ImGui::SetCursorPos(ImVec2(margin, marginTop));

    // Menu Title.
    ImGui::PushFont(ImGui::notoSansLargeFont);
    ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.00f), "Create a new mod");
    ImGui::PopFont();

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, spacing));

    // Configuration section.
    ImGui::PushFont(ImGui::notoSansMediumFont);
    ImGui::Text("Configuration");
    ImGui::PopFont();

    ImGui::PushFont(ImGui::notoSansNormalFont);

    ImGui::Dummy(ImVec2(0.0f, spacing));

    //  Text input for mod name.
    ImGui::SetNextItemWidth(availableWidth/4.f);
    ImGui::InputText("name", &m_ModName);

    // Text input for path to the mod directory.
    ImGui::SetNextItemWidth(3.0f*availableWidth/4.0f);
    if (ImGui::InputTextLocked("path", &m_ModPath)) {
        nfdchar_t *dirPath = NULL;
        nfdresult_t result = NFD_PickFolder(NULL, &dirPath);
            
        if(result == NFD_OKAY) {
            m_ModPath = std::string(dirPath);
        }
        else if(result != NFD_CANCEL) {
            // TODO: display errors.
        }
        free(dirPath);
    }

    // Combobox for the template preset (Default, Blank, Provinces, Heightmap...).
    ImGui::SetNextItemWidth(availableWidth/4.0f);
    if (ImGui::BeginCombo("template", TemplateTypeLabels.at(m_TemplateType).first.c_str())) {
        for (int i = 0; i < TemplateTypeLabels.size(); i++) {
            TemplateType type = (TemplateType) i;
            const bool isSelected = (m_TemplateType == type);
            if (ImGui::Selectable(TemplateTypeLabels.at(type).first.c_str(), isSelected))
                m_TemplateType = type;
            if(ImGui::IsItemHovered())
                ImGui::SetTooltip(TemplateTypeLabels.at(type).second.c_str());
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("?", "https://github.com/bombusfrigidus/Atlantis");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Each template is using Atlantis as the base structure.\nClick to open on GitHub.");

    if (m_TemplateType == TemplateType::PROVINCES_IMAGE || m_TemplateType == TemplateType::HEIGHTMAP_IMAGE) {
        // Text input for the path to the heightmap image.
        ImGui::SetNextItemWidth(3.0f*availableWidth/4.0f);
        if (ImGui::InputTextLocked("heightmap image", &m_HeightmapImagePath)) {
            nfdchar_t* filePath = NULL;
            nfdresult_t result = NFD_OpenDialog("png", m_ModPath.c_str(), &filePath);
                
            if(result == NFD_OKAY) {
                this->UpdateHeightmapImage(filePath);
            }
            else if(result != NFD_CANCEL) {
                // TODO: display errors.
            }
            free(filePath);
        }
    }
    if (m_TemplateType == TemplateType::PROVINCES_IMAGE) {
        // Text input for the path to the provinces image.
        ImGui::SetNextItemWidth(3.0f*availableWidth/4.0f);
        if (ImGui::InputTextLocked("provinces image", &m_ProvincesImagePath)) {
            nfdchar_t* filePath = NULL;
            nfdresult_t result = NFD_OpenDialog("png", m_ModPath.c_str(), &filePath);
                
            if(result == NFD_OKAY) {
                this->UpdateProvincesImage(filePath);
            }
            else if(result != NFD_CANCEL) {
                // TODO: display errors.
            }
            free(filePath);
        }
    }

    // Float input for the world water level (will be used to generate provinces).
    ImGui::SetNextItemWidth(availableWidth/4.0f);
    if (ImGui::InputFloat("water level", &m_WaterLevel, 0.1f, 1.0f, "%.3f")) {
        m_WaterLevel = std::max(0.0f, m_WaterLevel);
        this->UpdateLandmassTextures();
    }

    ImGui::PopFont();

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, spacing));

    // Preview section (display the expected maps).
    ImGui::PushFont(ImGui::notoSansMediumFont);
    ImGui::Text("Preview");
    ImGui::PopFont();

    float previewWidth = (windowSize.x-2*margin-5*ImGui::GetStyle().ItemSpacing.x) / 4.0f;
    float scale = previewWidth / std::max(1U, std::max(m_HeightmapTexture.getSize().x, m_ProvincesTexture.getSize().x));
    if (m_TemplateType == TemplateType::HEIGHTMAP_IMAGE || m_TemplateType == TemplateType::PROVINCES_IMAGE) {
        ImGui::Image(m_HeightmapTexture, sf::Vector2f(m_HeightmapTexture.getSize().x*scale, m_HeightmapTexture.getSize().y*scale));
        ImGui::SameLine();
        ImGui::Image(m_HeightmapLandmassTexture, sf::Vector2f(m_HeightmapLandmassTexture.getSize().x*scale, m_HeightmapLandmassTexture.getSize().y*scale));
    }
    if (m_TemplateType == TemplateType::PROVINCES_IMAGE) {
        ImGui::SameLine();
        ImGui::Image(m_ProvincesTexture, sf::Vector2f(m_ProvincesTexture.getSize().x*scale, m_ProvincesTexture.getSize().y*scale));
        ImGui::SameLine();
        ImGui::Image(m_ProvincesLandTexture, sf::Vector2f(m_ProvincesLandTexture.getSize().x*scale, m_ProvincesLandTexture.getSize().y*scale));
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::PushFont(ImGui::notoSansNormalFont);
    if (ImGui::TextButton("🔨 Create")) {

    }

    ImGui::Dummy(ImVec2(0.0f, 2*spacing));
    if (ImGui::TextButton("❌ Back")) {
        m_App->OpenMenu(MakeShared<HomeMenu>(m_App));
    }
    ImGui::PopFont();
    
    ImGui::NewLine();
    ImGui::End();
}

void NewModMenu::UpdateHeightmapImage(const std::string& filePath) {
    m_HeightmapImagePath = filePath;
    m_HeightmapTexture = sf::Texture();
    m_HeightmapLandmassTexture = sf::Texture();

    if (!m_HeightmapTexture.loadFromFile(m_HeightmapImagePath)) {
        // TODO: handle errors
        return;
    }

    this->UpdateLandmassTextures();
}

void NewModMenu::UpdateProvincesImage(const std::string& filePath) {
    m_ProvincesImagePath = filePath;
    m_ProvincesTexture = sf::Texture();
    m_ProvincesLandTexture = sf::Texture();

    if (!m_ProvincesTexture.loadFromFile(m_ProvincesImagePath)) {
        // TODO: handle errors
        return;
    }

    this->UpdateLandmassTextures();
}

void NewModMenu::UpdateLandmassTextures() {
    if (m_HeightmapTexture.getSize().x > 0) {
        // Update the landmass texture using the water level and the heightmap.
        sf::Shader& shader = Configuration::shaders.Get(Shaders::HEIGHTMAP_LANDMASS);
        shader.setUniform("texture", sf::Shader::CurrentTexture);
        shader.setUniform("waterLevel", m_WaterLevel);

        sf::RenderTexture renderTexture;
        if (!renderTexture.create(m_HeightmapTexture.getSize().x, m_HeightmapTexture.getSize().y))
            return;
        renderTexture.clear();

        sf::Sprite sprite(m_HeightmapTexture);
        renderTexture.draw(sprite, &shader);
        renderTexture.display();

        m_HeightmapLandmassTexture = renderTexture.getTexture();
    }
    
    if (m_ProvincesTexture.getSize().x > 0) {
        // Update the land provinces texture using the water level and the heightmap.
        sf::Shader& shader = Configuration::shaders.Get(Shaders::PROVINCES_LANDMASS);
        shader.setUniform("texture", sf::Shader::CurrentTexture);
        shader.setUniform("heightmapTexture", m_HeightmapTexture);
        shader.setUniform("waterLevel", m_WaterLevel);

        sf::RenderTexture renderTexture;
        if (!renderTexture.create(m_ProvincesTexture.getSize().x, m_ProvincesTexture.getSize().y))
            return;
        renderTexture.clear();

        sf::Sprite sprite(m_ProvincesTexture);
        renderTexture.draw(sprite, &shader);
        renderTexture.display();

        m_ProvincesLandTexture = renderTexture.getTexture();
    }
}