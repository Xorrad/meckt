#pragma once

class Title {
public:

    /**
     * @brief Construct a new Title object
     */
    Title();

    /**
     * @brief Construct a new Title object
     * @param name The name of the title
     * @param color The color of the title
     * @param landless Whether the title is landless (default: false)
     */
    Title(std::string name, sf::Color color, bool landless = false);

    virtual ~Title() = default;

    //////////////////////////////////////////////////////

    /**
     * @brief Checks if the title is of a specific type.
     * @param type The title type to check.
     * @return True if the title is of the specified type, false otherwise.
     */
    bool Is(TitleType type) const;

    /**
     * @brief Checks if the title is a vassal of a specific high title.
     * @param title The high title to check.
     * @return True if the title is a vassal of the specified high title, false otherwise.
     */
    bool IsVassal(HighTitle* title) const;

    /**
     * @brief Checks if the title has a localized name for a specific language.
     * @param lang The language to check.
     * @return True if the title has a localized name for the specified language, false otherwise.
     */
    bool HasLocName(const std::string& lang) const;

    /**
     * @brief Checks if the title has a localized adjective for a specific language.
     * @param lang The language to check.
     * @return True if the title has a localized adjective for the specified language, false otherwise.
     */
    bool HasLocAdjective(const std::string& lang) const;

    /**
     * @brief Checks if the title has a localized article for a specific language.
     * @param lang The language to check.
     * @return True if the title has a localized article for the specified language, false otherwise.
     */
    bool HasLocArticle(const std::string& lang) const;

    //////////////////////////////////////////////////////

    /** @brief Gets the type of the title.
     * @return The type of the title.
     */
    virtual TitleType GetType() const = 0;

    /**
     * @brief Gets the name of the title.
     * @return The name of the title.
     */
    std::string GetName() const;

    /**
     * @brief Gets the color of the title.
     * @return The color of the title.
     */
    sf::Color GetColor() const;

    /**
     * @brief Gets the liege title.
     * @return A pointer to the liege title.
     */
    HighTitle* GetLiegeTitle();

    /** @brief Gets the liege title of a specific type.
     * @param type The type of the liege title to get.
     * @return A pointer to the liege title of the specified type, nullptr if there is none.
     */
    HighTitle* GetLiegeTitle(TitleType type);

    /** @brief Checks if the title is landless.
     * @return True if the title is landless, false otherwise.
     */
    bool IsLandless() const;

    /** @brief Gets the original file name.
     * @note It excludes the path to the definition directory (common/landed_titles/)
     * @return The original file name.
     */
    std::string GetOriginalFileName() const;

    /** @brief Gets the original data.
     * @return A pointer to the original data.
     */
    SharedPtr<Jomini::Object> GetOriginalData() const;

    /** @brief Gets the original history file name.
     * @note It excludes the path to the definition directory (history/titles/)
     * @return The original history file name.
     */
    std::string GetOriginalHistoryFileName() const;

    /**
     * @brief Gets the history.
     * @return A reference to the history map.
     */
    std::map<Jomini::Date, SharedPtr<Jomini::Object>>& GetHistory();

    /**
     * @brief Gets the cultural names.
     * @return A reference to the cultural names map.
     */
    std::map<std::string, std::string>& GetCulturalNames();
    
    /**
     * @brief Gets the localized names.
     * @return A reference to the localized names map.
     */
    std::map<std::string, std::string>& GetLocNames();
    
    /**
     * @brief Gets the localized name for a specific language.
     * @param lang The language to get the name for.
     * @return A reference to the localized name for the specified language.
     */
    std::string& GetLocName(const std::string& lang);
    
    /**
     * @brief Gets the localized name for a specific language.
     * @param lang The language to get the name for.
     * @return The localized name for the specified language.
     */
    std::string GetLocName(const std::string& lang) const;

    /**
     * @brief Gets the localized adjectives.
     * @return A reference to the localized adjectives map.
     */
    std::map<std::string, std::string>& GetLocAdjectives();

    /**
     * @brief Gets the localized adjective for a specific language.
     * @param lang The language to get the adjective for.
     * @return A reference to the localized adjective for the specified language.
     */
    std::string& GetLocAdjective(const std::string& lang);

    /**
     * @brief Gets the localized adjective for a specific language.
     * @param lang The language to get the adjective for.
     * @return The localized adjective for the specified language.
     */
    std::string GetLocAdjective(const std::string& lang) const;
    
    /**
     * @brief Gets the localized articles.
     * @return A reference to the localized articles map.
     */
    std::map<std::string, std::string>& GetLocArticles();
    
    /**
     * @brief Gets the localized article for a specific language.
     * @param lang The language to get the article for.
     * @return A reference to the localized article for the specified language.
     */
    std::string& GetLocArticle(const std::string& lang);

    /**
     * @brief Gets the localized article for a specific language.
     * @param lang The language to get the article for.
     * @return The localized article for the specified language.
     */
    std::string GetLocArticle(const std::string& lang) const;

    /**
     * @brief Checks if the title has selection focus.
     * @return True if the title has selection focus, false otherwise.
     */
    virtual bool HasSelectionFocus() const;

    /**
     * @brief Gets the title's position in the provinces image.
     * @param provinceManager The province manager.
     * @return The title position.
     */
    virtual sf::Vector2i GetImagePosition(const ProvinceManager& provinceManager) const = 0;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the title's name.
     * @param name The name to set.
     */
    void SetName(std::string name);

    /**
     * @brief Sets the title's color.
     * @param color The color to set.
     */
    void SetColor(sf::Color color);

    /**
     * @brief Sets the title's liege title.
     * @param title The liege title to set.
     */
    void SetLiegeTitle(HighTitle* title);

    /**
     * @brief Sets the title's landless status.
     * @param landless The landless status to set.
     */
    void SetLandless(bool landless);
    
    /**
     * @brief Sets the title's original file name.
     * @param fileName The original file name to set.
     */
    void SetOriginalFileName(const std::string& fileName);

    /**
     * @brief Sets the title's original data.
     * @param data The original data to set.
     */
    void SetOriginalData(SharedPtr<Jomini::Object> data);

    /**
     * @brief Sets the title's original history file name.
     * @param fileName The original history file name to set.
     */
    void SetOriginalHistoryFileName(const std::string& fileName);

    /**
     * @brief Sets the localized name for a specific language.
     * @param lang The language to set the name for.
     * @param name The name to set.
     */
    void SetLocName(const std::string& lang, std::string name);

    /**
     * @brief Sets the localized adjective for a specific language.
     * @param lang The language to set the adjective for.
     * @param adjective The adjective to set.
     */
    void SetLocAdjective(const std::string& lang, std::string adjective);

    /**
     * @brief Sets the localized article for a specific language.
     * @param lang The language to set the article for.
     * @param article The article to set.
     */
    void SetLocArticle(const std::string& lang, std::string article);

    /**
     * @brief Sets the title's selection focus.
     * @param focus The selection focus to set.
     */
    virtual void SetSelectionFocus(bool focus);

    //////////////////////////////////////////////////////
    
    /**
     * @brief Adds a history entry for the title.
     * @param date The date of the history entry.
     * @param data The history data to add.
     */
    void AddHistory(Jomini::Date date, SharedPtr<Jomini::Object> data);
    
    /**
     * @brief Removes a history entry for the title.
     * @param date The date of the history entry to remove.
     */
    void RemoveHistory(Jomini::Date date);
    
    /**
     * @brief Adds a cultural name for the title.
     * @param culture The culture to set the name for.
     * @param name The name to set.
     */
    void AddCulturalName(const std::string& culture, std::string name);

    /**
     * @brief Removes a cultural name for the title.
     * @param culture The culture to remove the name for.
     */
    void RemoveCulturalName(const std::string& culture);

    //////////////////////////////////////////////////////

protected:
    std::string m_Name;
    sf::Color m_Color;
    HighTitle* m_LiegeTitle;
    bool m_Landless;

    std::string m_OriginalFileName;
    SharedPtr<Jomini::Object> m_OriginalData;

    std::string m_OriginalHistoryFileName;
    std::map<Jomini::Date, SharedPtr<Jomini::Object>> m_History;
    std::map<std::string, std::string> m_CulturalNames;

    std::map<std::string, std::string> m_LocNames;
    std::map<std::string, std::string> m_LocAdjectives;
    std::map<std::string, std::string> m_LocArticles;

    bool m_SelectionFocus;
};

#include "HighTitle.hpp"
#include "types/BaronyTitle.hpp"
#include "types/CountyTitle.hpp"
#include "types/DuchyTitle.hpp"
#include "types/KingdomTitle.hpp"
#include "types/EmpireTitle.hpp"
#include "types/HegemonyTitle.hpp"

template <typename ...Args>
inline UniquePtr<Title> MakeTitle(TitleType type, Args&& ...args) {
    switch(type) {
        case TitleType::BARONY: return MakeUnique<BaronyTitle>(std::forward<Args>(args)...);
        case TitleType::COUNTY: return MakeUnique<CountyTitle>(std::forward<Args>(args)...);
        case TitleType::DUCHY: return MakeUnique<DuchyTitle>(std::forward<Args>(args)...);
        case TitleType::KINGDOM: return MakeUnique<KingdomTitle>(std::forward<Args>(args)...);
        case TitleType::EMPIRE: return MakeUnique<EmpireTitle>(std::forward<Args>(args)...);
        case TitleType::HEGEMONY: return MakeUnique<HegemonyTitle>(std::forward<Args>(args)...);
        default: break;
    }
    throw std::invalid_argument("MakeTitle: invalid title type");
}