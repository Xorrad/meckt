#pragma once

#include <sol/sol.hpp>

namespace Script {

/**
 * @brief One documented entry of the scripting API, as listed by the script
 *        executor's "Docs" panel.
 */
struct DocEntry {
    // Table or usertype the entry belongs to, e.g. "provinces" or "Province".
    std::string scope;
    // Function name on its own, e.g. "GetById".
    std::string name;
    // Full call signature, e.g. "provinces.GetById(id) -> Province|nil".
    std::string signature;
    // One-line explanation of what the function does.
    std::string description;
};

/**
 * @brief The scripting API reference.
 *
 * Entries are added by SCRIPT_FN() at the point the binding itself is declared,
 * so a function cannot be exposed to Lua without also showing up in the docs.
 */
class Docs {
public:
    /**
     * @brief Adds an entry to the reference. Prefer SCRIPT_FN() over calling
     *        this directly, so the entry stays next to the binding it describes.
     */
    void Register(std::string scope, std::string name, std::string signature, std::string description);

    /**
     * @brief Removes every registered entry.
     */
    void Clear();

    /**
     * @brief Retrieves every registered entry, in registration order.
     */
    const std::vector<DocEntry>& GetEntries() const;

    /**
     * @brief Retrieves every entry belonging to the given scope, in registration order.
     * @param scope The table or usertype name, e.g. "provinces".
     */
    std::vector<const DocEntry*> GetEntries(const std::string& scope) const;

    /**
     * @brief Retrieves the distinct scopes that have at least one entry, in the
     *        order they were first registered.
     */
    std::vector<std::string> GetScopes() const;

    /**
     * @brief Counts the total number of registered entries.
     */
    size_t CountEntries() const;

    /**
     * @brief Renders the whole reference as plain text, grouped by scope.
     */
    std::string ToString() const;

private:
    std::vector<DocEntry> m_Entries;
};

/**
 * @brief Pairs a Lua table (or usertype) with the scope name it's documented
 *        under, so SCRIPT_FN() can bind a function and document it in a single
 *        statement.
 * @tparam T The underlying sol type (`sol::table` or `sol::usertype<...>`).
 */
template <typename T>
class Binder {
public:
    Binder(Docs& docs, T table, std::string scope) :
        m_Docs(docs),
        m_Table(std::move(table)),
        m_Scope(std::move(scope))
    {}

    /**
     * @brief Binds `fn` as `<scope>.<name>` and registers it in the docs.
     * @note  Prefer the SCRIPT_FN() macro, which reads better at the call site.
     */
    template <typename F>
    Binder& Bind(const std::string& name, const std::string& signature, const std::string& description, F&& fn) {
        m_Docs.Register(m_Scope, name, signature, description);
        m_Table[name] = std::forward<F>(fn);
        return *this;
    }

    /**
     * @brief Retrieves the underlying sol table/usertype, for the rare binding
     *        that can't go through Bind() (e.g. metamethods, which aren't
     *        callable from a script by name and so aren't documented).
     */
    T& GetTable() { return m_Table; }

private:
    Docs& m_Docs;
    T m_Table;
    std::string m_Scope;
};

/**
 * @brief Creates a Binder for a sol table/usertype, deducing its type.
 * @param docs The reference the bound functions get registered into.
 * @param table The sol table or usertype to bind onto.
 * @param scope The name the scope is documented (and reachable from Lua) under.
 */
template <typename T>
inline Binder<T> MakeBinder(Docs& docs, T table, std::string scope) {
    return Binder<T>(docs, std::move(table), std::move(scope));
}

}

/**
 * @brief Binds a function into the Lua API and adds it to the docs listing in
 *        one statement, so an exposed function is always a documented one.
 * @param binder The Script::Binder for the target table/usertype (see MakeBinder()).
 * @param name The function name as scripts call it, e.g. "GetById".
 * @param signature The full call signature, e.g. "provinces.GetById(id) -> Province|nil".
 * @param description A one-line explanation of what the function does.
 * @param ... The bound callable: a member pointer, a lambda, or an overload set.
 */
#define SCRIPT_FN(binder, name, signature, description, ...) \
    (binder).Bind(name, signature, description, __VA_ARGS__)
