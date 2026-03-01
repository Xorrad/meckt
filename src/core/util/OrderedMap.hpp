#pragma once

template <typename K, typename V>
class OrderedMap {
public:
    void insert(const K& key, const V& value) {
        if (m_Index.find(key) == m_Index.end()) {
            m_Items.emplace_back(key, value);
            m_Index[key] = --m_Items.end();
        } else {
            m_Index[key]->second = value;
        }
    }

    V& at(const K& key) {
        if (m_Index.find(key) == m_Index.end()) {
            throw std::out_of_range(std::format("OrderedMap: key not found for \"{}\"", key));
        }
        return m_Index[key]->second;
    }

    V& operator[](const K& key) {
        if (m_Index.find(key) == m_Index.end()) {
            m_Items.emplace_back(key, V());
            m_Index[key] = --m_Items.end();
        }
        return m_Index[key]->second;
    }

    const V& operator[](const K& key) const {
        static V default_value{};
        auto it = m_Index.find(key);
        return (it != m_Index.end()) ? it->second->second : default_value;
    }

    typename std::list<std::pair<K, V>>::iterator begin() {
        return m_Items.begin();
    }

    typename std::list<std::pair<K, V>>::iterator end() {
        return m_Items.end();
    }

    typename std::list<std::pair<K, V>>::const_iterator begin() const {
        return m_Items.cbegin();
    }

    typename std::list<std::pair<K, V>>::const_iterator end() const {
        return m_Items.cend();
    }

    std::size_t size() const {
        return m_Items.size();
    }

    bool empty() const {
        return m_Items.size() == 0;
    }

    void erase(const K& key) {
        auto it = m_Index.find(key);
        if (it != m_Index.end()) {
            m_Items.erase(it->second);
            m_Index.erase(it);
        }
    }

    typename std::list<std::pair<K, V>>::iterator find(const K& key) {
        auto it = m_Index.find(key);
        return (it != m_Index.end()) ? it->second : m_Items.end();
    }

    bool contains(const K& key) const {
        return m_Index.count(key) > 0;
    }

    std::vector<K> keys() const {
        std::vector<K> keys;
        for(auto [key, value] : m_Items)
            keys.push_back(key);
        return keys;
    }

    void clear() {
        m_Items.clear();
        m_Index.clear();
    }
private:
    using ListIterator = typename std::list<std::pair<K, V>>::iterator;
    std::list<std::pair<K, V>> m_Items;
    std::map<K, ListIterator> m_Index;
};