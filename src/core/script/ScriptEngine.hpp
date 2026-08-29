#pragma once

#include "script/ScriptDocs.hpp"

namespace Script {

/**
 * @brief The outcome of running a script.
 */
struct Result {
    // False if the script raised an error (syntax or runtime).
    bool success = false;
    // The Lua error message, empty when `success` is true.
    std::string error;
    // Everything the script wrote through print()/log(), one entry per call.
    std::vector<std::string> output;
};

/**
 * @brief Runs user-written Lua scripts against a mod's data.
 *
 * The whole API surface is bound in the constructor and simultaneously recorded
 * in GetDocs() (see SCRIPT_FN), so what a script can call and what the docs list
 * can't drift apart.
 */
class Engine {
public:
    // Scripts are aborted past this many VM instructions, so a runaway loop in a
    // user script can't hang the app with no way out.
    static constexpr int DefaultInstructionLimit = 50'000'000;

    Engine(Mod& mod);
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    /**
     * @brief Runs a script against the mod, capturing its output and any error.
     * @note  Never throws: Lua errors (including the instruction-limit abort)
     *        are reported through the returned result instead.
     * @param source The Lua source to run.
     * @return The output the script produced, and the error that stopped it, if any.
     */
    Result Run(const std::string& source);

    /**
     * @brief Retrieves the underlying Lua state, so callers can bind their own
     *        additional API on top of the core one (see EditorMenu, which adds
     *        the map-mode and selection bindings it alone knows about).
     */
    sol::state& GetState();

    /**
     * @brief Retrieves the API reference every bound function is registered in.
     */
    Docs& GetDocs();
    const Docs& GetDocs() const;

    /**
     * @brief Sets how many VM instructions a script may run before being aborted.
     * @param limit The instruction budget, or 0 to disable the limit entirely.
     */
    void SetInstructionLimit(int limit);
    int GetInstructionLimit() const;

    /**
     * @brief Checks whether a script changed data the rendered map is built from
     *        (province colors/flags, titles, cultures...), meaning the caller
     *        should rebuild its textures. Cleared at the start of every Run().
     */
    bool IsMapInvalidated() const;

    /**
     * @brief Flags the map as needing a rebuild. Called by the bindings that
     *        mutate map-visible data; also callable from a script via
     *        `map.Invalidate()` after an edit the bindings can't detect.
     */
    void InvalidateMap();

    /**
     * @brief Appends a line to the current run's captured output. Used by the
     *        print()/log() bindings, and by any API added on top of this one.
     */
    void Print(const std::string& text);

private:
    void BindUtility();
    void BindTypes();
    void BindManagers();
    void BindMap();

private:
    Mod& m_Mod;
    sol::state m_State;
    Docs m_Docs;

    std::vector<std::string> m_Output;
    int m_InstructionLimit;
    bool m_MapInvalidated;
};

}
