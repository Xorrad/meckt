#pragma once

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(Resources);

template <typename R, typename I = int>
class ResourceManager {
public:
    ResourceManager(const std::string& name) : m_Name(name) {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    template <typename ...Args>
    R& Load(const I& id, const std::string& path, Args&& ...args) {
        auto ptr = std::make_unique<R>();
        bool success = false;

        #ifdef DEBUG
            // Standard disk loading.
            success = this->Load(*ptr, false, path, std::forward<Args>(args)...);
            std::string source = "file " + path;
        #else
            // Embedded loading via CMRC.
            auto fs = cmrc::Resources::get_filesystem();
            auto file = fs.open(path);
            std::string_view data(file.begin(), std::distance(file.begin(), file.end()));
            
            success = this->Load(*ptr, true, data, std::forward<Args>(args)...);
            std::string source = "memory " + path;
        #endif

        if (!success) {
            LOG_ERROR("Failed to load resource {} (ID: {}) from {}", m_Name, (int) id, source);
            throw std::runtime_error("ResourceManager: resource loading failed: " + path);
        }

        return *(m_Resources.emplace(id, std::move(ptr)).first->second);
    }

    R& Get(const I& id) const {
        return *m_Resources.at(id);
    }

    void Clear() {
        m_Resources.clear();
    }

private:
    template <typename T, typename... Args>
    bool Load(T& resource, bool fromMemory, std::string_view data, Args&&... args) {
        if constexpr (std::is_same_v<T, sf::Music> || std::is_same_v<T, sf::Font>) {
            if (fromMemory)
                return resource.openFromMemory(data.data(), data.size(), std::forward<Args>(args)...);
            return resource.openFromFile(data, std::forward<Args>(args)...);
        }
        else if constexpr (std::is_same_v<T, sf::Shader>) {
            if (fromMemory) {
                if constexpr (sizeof...(args) > 0) {
                    // Dual Shaders (path1, path2)
                    if constexpr (std::is_convertible_v<std::tuple_element_t<0, std::tuple<Args...>>, std::string>) {
                        std::string secondPath = std::get<0>(std::forward_as_tuple(args...));
                        auto fs = cmrc::Resources::get_filesystem();
                        auto file = fs.open(secondPath);
                        std::string secondSource = std::string(file.begin(), file.end());

                        return resource.loadFromMemory(data, secondSource);
                    }

                    // Single Shader (path, sf::Shader::Type)
                    return resource.loadFromMemory(data, std::forward<Args>(args)...);
                }

                return resource.loadFromMemory(data, std::forward<Args>(args)...);
            }
            return resource.loadFromFile(data, std::forward<Args>(args)...);
        }
        else {
            if (fromMemory)
                return resource.loadFromMemory(data.data(), data.size(), std::forward<Args>(args)...);
            return resource.loadFromFile(data, std::forward<Args>(args)...);
        }
    }

private:
    std::string m_Name;
    std::unordered_map<I, std::unique_ptr<R>> m_Resources;
};