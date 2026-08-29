#pragma once

#include "core/script/ScriptEngine.hpp"

/**
 * @brief The Lua script executor modal (Tools > Run a Lua script).
 *
 * Owns the mod-bound Script::Engine, adds the editor-only bindings the core
 * engine can't know about (map modes, the current selection), and renders the
 * editor/output/reference panels.
 */
class ScriptModal {
public:
    // The name the modal is opened and rendered under (see EditorMenu::m_ModalName).
    static constexpr const char* Name = "Run a Lua script";

    ScriptModal(EditorMenu& menu);
    ScriptModal(const ScriptModal&) = delete;
    ScriptModal& operator=(const ScriptModal&) = delete;

    /**
     * @brief Renders the modal, if it's currently open. Call unconditionally
     *        from EditorMenu::RenderModals().
     */
    void Render();

    /**
     * @brief Runs whatever is currently in the editor, capturing its output and
     *        refreshing the map if the script changed anything drawn on it.
     */
    void RunScript();

    /**
     * @brief Retrieves the underlying engine, e.g. to bind more API onto it.
     */
    Script::Engine& GetEngine();

private:
    void BindEditorApi();

    void RenderToolbar();
    void RenderEditor(const ImVec2& size);
    void RenderDocs(const ImVec2& size);
    void RenderOutput(const ImVec2& size);

private:
    EditorMenu& m_Menu;
    Script::Engine m_Engine;

    std::string m_Source;
    // Output lines of the last run, plus the error that ended it (if any).
    std::vector<std::string> m_Output;
    std::string m_Error;
    bool m_HasRun;

    // Case-insensitive filter applied to the reference panel.
    std::string m_DocsFilter;
};
