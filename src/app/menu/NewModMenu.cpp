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
    m_WaterLevel(3.8f),
    m_IsCreating(false),
    m_CreationState(CreationState::CLONING)
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

    if (!m_IsCreating)  {
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
            m_CreationThread = MakeShared<sf::Thread>([&]() {
                this->CreateMod();
            });
            m_CreationThread->launch();
        }

        ImGui::Dummy(ImVec2(0.0f, 2*spacing));
        if (ImGui::TextButton("❌ Back")) {
            m_App->OpenMenu(MakeShared<HomeMenu>(m_App));
        }
        ImGui::PopFont();
    }
    else {
        // Configuration section.
        ImGui::PushFont(ImGui::notoSansMediumFont);
        ImGui::Text("Creating project...");
        ImGui::PopFont();

        ImGui::PushFont(ImGui::notoSansNormalFont);
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetColorU32(ImGuiCol_Button));
        ImGui::ProgressBar(((float) m_CreationState)/(CreationStateLabels.size()-1), ImVec2(0.0f, 0.0f), CreationStateLabels.at(m_CreationState).c_str());
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }
    
    ImGui::NewLine();
    ImGui::End();

    // Open the created mod once the generation process is finished.
    if (m_CreationState == CreationState::FINISHED) {
        SharedPtr<Mod> mod = MakeShared<Mod>(m_ModPath);
        m_App->OpenMod(mod);
    }
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

void NewModMenu::CreateMod() {
    if (std::filesystem::exists(m_ModPath))
        return;

    m_IsCreating = true;
    std::filesystem::path modPath = std::filesystem::path(m_ModPath);
    std::filesystem::create_directories(modPath);

    // Clone Atlantis into the mod directory.
    m_CreationState = CreationState::CLONING;
    std::filesystem::path parentPath = std::filesystem::path(m_ModPath).parent_path();
    std::filesystem::path atlantisPath = parentPath / "atlantis.zip";
    File::DownloadFile(Configuration::atlantisURL, atlantisPath);
    // TODO: handle errors.

    // Unzip the files.
    m_CreationState = CreationState::UNZIPPING;
    File::UnzipFile(atlantisPath, parentPath);

    // Setup the Atlantis template.
    m_CreationState = CreationState::SETTING_UP;
    std::rename((parentPath / "Atlantis-main").c_str(), modPath.c_str());
    std::remove((modPath / "Atlantis.code-workspace").c_str());
    std::remove((modPath / "Atlantis.mod").c_str());
    std::remove((modPath / "README.md").c_str());
    std::remove((modPath / ".gitattributes").c_str());
    std::remove((modPath / ".gitattributes").c_str());
    std::remove(atlantisPath.c_str());

    SharedPtr<Jomini::Object> descriptorData = Jomini::ParseFile(modPath / "descriptor.mod");
    descriptorData->Put("name", "\"" + m_ModName + "\"");
    std::ofstream descriptorFile(modPath / "descriptor.mod", std::ios::out);
    descriptorFile << descriptorData->Serialize();
    descriptorFile.close();

    // Remove spaces and quotes from the mod name for the main descriptor file.
    std::string modFileName = m_ModName;
    std::replace(modFileName.begin(), modFileName.end(), ' ', '_');
    std::replace(modFileName.begin(), modFileName.end(), '\'', '_');
    descriptorData->Put("path", "\"" + modPath.string() + "\"");
    descriptorFile.open(modPath / std::string(modFileName + ".mod"), std::ios::out);
    descriptorFile << descriptorData->Serialize();
    descriptorFile.close();

    // Copy heightmap and provinces images into the mod directory.
    if (m_TemplateType == TemplateType::HEIGHTMAP_IMAGE || m_TemplateType == TemplateType::PROVINCES_IMAGE) {
        m_CreationState = CreationState::COPYING_IMAGES;
        sf::sleep(sf::seconds(1));
    }
    
    // Generate the world provinces using the heightmap to determine the landmass.
    // TODO: intermediate preview of the generation?
    if (m_TemplateType == TemplateType::HEIGHTMAP_IMAGE) {
        m_CreationState = CreationState::GENERATING_WORLD;
        sf::sleep(sf::seconds(1));
    }
    
    // Generate a province for each color in the provinces image.
    if (m_TemplateType == TemplateType::HEIGHTMAP_IMAGE || m_TemplateType == TemplateType::PROVINCES_IMAGE) {
        m_CreationState = CreationState::GENERATING_PROVINCES;
        sf::sleep(sf::seconds(1));
    }
    
    // Determine the type (land, sea...) of each province using the heightmap and water level.
    // Not done when generating world because the landmass has already been determined.
    if (m_TemplateType == TemplateType::PROVINCES_IMAGE) {
        m_CreationState = CreationState::GENERATE_TERRAIN;
        sf::sleep(sf::seconds(1));
    }
    
    m_CreationState = CreationState::FINISHED;
}