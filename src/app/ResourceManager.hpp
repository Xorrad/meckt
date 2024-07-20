#pragma once

template <typename R, typename I = int>
class ResourceManager {
public:
    ResourceManager(const std::string& name) : m_Name(name) {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void Load(const I& id, const std::string& path) {
        std::unique_ptr<R> ptr(new R());
        if(!ptr->loadFromFile(path))
            throw std::runtime_error("failed to load resource file.");
        m_Resources.emplace(id, std::move(ptr));
        INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }
    
    template <typename ...Args>
    void Load(const I& id, const std::string& path, Args&& ...args) {
        std::unique_ptr<R> ptr(new R());
        if(!ptr->loadFromFile(path, std::forward<Args>(args)...))
            throw std::runtime_error("failed to load resource file.");
        m_Resources.emplace(id, std::move(ptr));
        INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }

    R& Get(const I& id) const {
        return *m_Resources.at(id);
    }

private:
    std::string m_Name;
    std::unordered_map<I, std::unique_ptr<R>> m_Resources;
};

template<typename I>
class ResourceManager<sf::Music, I>
{
public:
    ResourceManager() : m_Name("Music") {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void Load(const I& id, const std::string& path) {
        std::unique_ptr<sf::Music> ptr(new sf::Music());
        if(!ptr->openFromFile(path))
            throw std::runtime_error("failed to load music file.");
        m_Resources.emplace(id, std::move(ptr));
        INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }
    
    template<typename ...Args>
    void Load(const I& id, const std::string& path, Args&& ...args) {
        std::unique_ptr<sf::Music> ptr(new sf::Music());
        if(!ptr->openFromFile(path, std::forward<Args>(args)...))
            throw std::runtime_error("failed to load music file.");
        m_Resources.emplace(id, std::move(ptr));
        INFO("loaded {} {} from {}", m_Name, (int) id, path);
    }

    sf::Music& Get(const I& id) const {
        return *m_Resources.at(id);
    }

private:
    std::string m_Name;
    std::unordered_map<I, std::unique_ptr<sf::Music>> m_Resources;
};