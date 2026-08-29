#include "ScriptDocs.hpp"

namespace Script {

void Docs::Register(std::string scope, std::string name, std::string signature, std::string description) {
    m_Entries.push_back(DocEntry{
        std::move(scope),
        std::move(name),
        std::move(signature),
        std::move(description)
    });
}

void Docs::Clear() {
    m_Entries.clear();
}

const std::vector<DocEntry>& Docs::GetEntries() const {
    return m_Entries;
}

std::vector<const DocEntry*> Docs::GetEntries(const std::string& scope) const {
    std::vector<const DocEntry*> entries;
    for (const DocEntry& entry : m_Entries) {
        if (entry.scope == scope)
            entries.push_back(&entry);
    }
    return entries;
}

std::vector<std::string> Docs::GetScopes() const {
    std::vector<std::string> scopes;
    std::unordered_set<std::string> visited;

    for (const DocEntry& entry : m_Entries) {
        if (visited.insert(entry.scope).second)
            scopes.push_back(entry.scope);
    }
    return scopes;
}

size_t Docs::CountEntries() const {
    return m_Entries.size();
}

std::string Docs::ToString() const {
    std::string text;

    for (const std::string& scope : this->GetScopes()) {
        text += fmt::format("--- {} ---\n", scope);

        for (const DocEntry* entry : this->GetEntries(scope))
            text += fmt::format("{}\n    {}\n", entry->signature, entry->description);

        text += "\n";
    }
    return text;
}

}
