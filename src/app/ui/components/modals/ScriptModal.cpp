#include "ScriptModal.hpp"

#include "app/App.hpp"
#include "app/menu/EditorMenu.hpp"

#include "core/mod/Mod.hpp"
#include "core/provinces/ProvinceManager.hpp"

#include <imgui/imgui.hpp>

namespace {

// Shown the first time the modal is opened, as a hint at the API's shape.
constexpr const char* DefaultSource =
    "-- Counts the land provinces that have no barony.\n"
    "local orphans = 0\n"
    "\n"
    "for _, province in ipairs(provinces.GetAll()) do\n"
    "    if province:HasFlag(ProvinceFlags.LAND) and province:GetBarony() == nil then\n"
    "        orphans = orphans + 1\n"
    "    end\n"
    "end\n"
    "\n"
    "print(orphans .. \" land provinces without a barony\")\n";

bool ContainsCaseInsensitive(const std::string& haystack, const std::string& needle) {
    const auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](char a, char b) { return std::tolower(a) == std::tolower(b); }
    );
    return it != haystack.end();
}

}

ScriptModal::ScriptModal(EditorMenu& menu) :
    m_Menu(menu),
    m_Engine(menu.GetApp().GetMod()),
    m_Source(DefaultSource),
    m_Output({}),
    m_Error(""),
    m_HasRun(false),
    m_DocsFilter("")
{
    this->BindEditorApi();
}

Script::Engine& ScriptModal::GetEngine() {
    return m_Engine;
}

void ScriptModal::BindEditorApi() {
    sol::state& state = m_Engine.GetState();

    state.new_enum<MapMode>("MapMode", {
        { "PROVINCES",       MapMode::PROVINCES },
        { "HEIGHTMAP",       MapMode::HEIGHTMAP },
        { "RIVERS",          MapMode::RIVERS },
        { "FLAGS",           MapMode::FLAGS },
        { "TERRAIN",         MapMode::TERRAIN },
        { "CLIMATE",         MapMode::CLIMATE },
        { "WINTER_SEVERITY", MapMode::WINTER_SEVERITY },
        { "CULTURE",         MapMode::CULTURE },
        { "FAITH",           MapMode::FAITH },
        { "BARONY",          MapMode::BARONY },
        { "COUNTY",          MapMode::COUNTY },
        { "DUCHY",           MapMode::DUCHY },
        { "KINGDOM",         MapMode::KINGDOM },
        { "EMPIRE",          MapMode::EMPIRE },
        { "HEGEMONY",        MapMode::HEGEMONY },
    });

    auto editor = Script::MakeBinder(m_Engine.GetDocs(), state.create_named_table("editor"), "editor");

    SCRIPT_FN(editor, "GetMapMode", "editor.GetMapMode() -> MapMode",
        "Returns the map mode currently displayed.",
        [this]() { return m_Menu.GetMapMode(); });

    SCRIPT_FN(editor, "SetMapMode", "editor.SetMapMode(mode)",
        "Switches the displayed map mode (e.g. MapMode.CULTURE).",
        [this](MapMode mode) { m_Menu.SwitchMapMode(mode, false); });

    SCRIPT_FN(editor, "GetMapModeName", "editor.GetMapModeName(mode) -> string",
        "Returns a map mode's readable name (e.g. \"Winter Severity\").",
        [](MapMode mode) {
            int index = static_cast<int>(mode);
            if (index < 0 || index >= static_cast<int>(MapMode::COUNT))
                throw std::runtime_error("invalid map mode");

            return std::string(MapModeLabels[index]);
        });

    SCRIPT_FN(editor, "GetSelectedProvinces", "editor.GetSelectedProvinces() -> Province[]",
        "Returns the provinces currently selected on the map.",
        [this]() {
            sol::table array = m_Engine.GetState().create_table();
            int index = 1;
            for (Province* province : m_Menu.GetSelectionHandler().GetProvinces())
                array[index++] = province;
            return array;
        });

    SCRIPT_FN(editor, "GetSelectedTitles", "editor.GetSelectedTitles() -> Title[]",
        "Returns the titles currently selected on the map.",
        [this]() {
            sol::table array = m_Engine.GetState().create_table();
            int index = 1;
            for (Title* title : m_Menu.GetSelectionHandler().GetTitles())
                array[index++] = title;
            return array;
        });

    SCRIPT_FN(editor, "Select", "editor.Select(province)",
        "Adds a province to the map selection.",
        [this](Province* province) { m_Menu.GetSelectionHandler().Select(province); });

    SCRIPT_FN(editor, "ClearSelection", "editor.ClearSelection()",
        "Clears the whole map selection.",
        [this]() { m_Menu.GetSelectionHandler().ClearSelection(); });
}

//////////////////////////////////////////////////////

void ScriptModal::RunScript() {
    Script::Result result = m_Engine.Run(m_Source);

    m_Output = result.output;
    m_Error = result.error;
    m_HasRun = true;

    // Province geometry changes invalidate the render index the palettes are
    // addressed by, so the textures have to be rebuilt from scratch.
    if (m_Engine.IsMapInvalidated()) {
        m_Menu.GetApp().GetMod().GetProvinceManager().BuildProvinceIndex();
        m_Menu.UpdateTextures();
        m_Menu.RefreshCurrentMapMode(false);
    }
}

void ScriptModal::Render() {
    ImVec2 viewport = ImGui::GetMainViewport()->Size;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(viewport.x * 0.75f, viewport.y * 0.75f), ImGuiCond_Appearing);

    if (!ImGui::BeginPopupModal(Name, nullptr, ImGuiWindowFlags_NoMove))
        return;

    this->RenderToolbar();
    ImGui::Separator();

    // The editor and the reference share the upper area; the output console
    // takes a fixed slice of the bottom, above the Close button.
    float footerHeight = ImGui::GetFrameHeightWithSpacing() * 2.f;
    float outputHeight = ImGui::GetContentRegionAvail().y * 0.3f;
    float upperHeight = ImGui::GetContentRegionAvail().y - outputHeight - footerHeight;
    float editorWidth = ImGui::GetContentRegionAvail().x * 0.6f;

    this->RenderEditor(ImVec2(editorWidth, upperHeight));
    ImGui::SameLine();
    this->RenderDocs(ImVec2(0, upperHeight));

    this->RenderOutput(ImVec2(0, outputHeight));

    if (ImGui::Button("Close"))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

void ScriptModal::RenderToolbar() {
    if (ImGui::Button("Run"))
        this->RunScript();

    ImGui::SameLine();
    if (ImGui::Button("Clear output")) {
        m_Output.clear();
        m_Error.clear();
        m_HasRun = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear script"))
        m_Source.clear();

    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(
            "Scripts edit the mod in memory: nothing is written to disk until you export.\n"
            "There is no undo, so work on a backup of your mod."
        );
    }

    ImGui::SameLine();
    ImGui::TextDisabled("| %zu functions available", m_Engine.GetDocs().CountEntries());
}

void ScriptModal::RenderEditor(const ImVec2& size) {
    if (ImGui::BeginChild("script-editor", size, ImGuiChildFlags_Borders)) {
        ImGui::InputTextMultiline(
            "##source",
            &m_Source,
            ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y),
            ImGuiInputTextFlags_AllowTabInput
        );
    }
    ImGui::EndChild();
}

void ScriptModal::RenderDocs(const ImVec2& size) {
    if (ImGui::BeginChild("script-docs", size, ImGuiChildFlags_Borders)) {
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputTextWithHint("##docs-filter", "Filter the API...", &m_DocsFilter);

        const Script::Docs& docs = m_Engine.GetDocs();

        for (const std::string& scope : docs.GetScopes()) {
            std::vector<const Script::DocEntry*> entries = docs.GetEntries(scope);

            // Keep a scope visible when its own name matches, so filtering by
            // "provinces" shows the whole table rather than nothing.
            bool scopeMatches = m_DocsFilter.empty() || ContainsCaseInsensitive(scope, m_DocsFilter);

            std::vector<const Script::DocEntry*> matching;
            for (const Script::DocEntry* entry : entries) {
                if (scopeMatches
                    || ContainsCaseInsensitive(entry->name, m_DocsFilter)
                    || ContainsCaseInsensitive(entry->description, m_DocsFilter)) {
                    matching.push_back(entry);
                }
            }

            if (matching.empty())
                continue;

            if (!m_DocsFilter.empty())
                ImGui::SetNextItemOpen(true, ImGuiCond_Always);

            // The entry count is part of the label but not of the id (###),
            // which would otherwise change as the filter narrows the list and
            // reset the header's open/closed state along with it.
            std::string header = fmt::format("{} ({})###{}", scope, matching.size(), scope);

            if (ImGui::CollapsingHeader(header.c_str())) {
                for (const Script::DocEntry* entry : matching) {
                    ImGui::Bullet();
                    ImGui::TextUnformatted(entry->signature.c_str());

                    ImGui::Indent();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
                    ImGui::TextWrapped("%s", entry->description.c_str());
                    ImGui::PopStyleColor();
                    ImGui::Unindent();
                }
            }
        }
    }
    ImGui::EndChild();
}

void ScriptModal::RenderOutput(const ImVec2& size) {
    if (ImGui::BeginChild("script-output", size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar)) {
        for (const std::string& line : m_Output)
            ImGui::TextUnformatted(line.c_str());

        if (!m_Error.empty())
            ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), "%s", m_Error.c_str());
        else if (m_HasRun && m_Output.empty())
            ImGui::TextDisabled("The script ran without producing any output.");
        else if (!m_HasRun)
            ImGui::TextDisabled("Output will appear here.");
    }
    ImGui::EndChild();
}
