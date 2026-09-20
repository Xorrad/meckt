namespace Components {

    /**
     * A custom input field for editing a title's name.
     * @param label The label for the input field.
     * @param currentName The current name of the title.
     * @param onChange A function to be called when the name is changed.
     * @param type The type of the title (e.g., barony, county...).
     * @return True if the name was changed, false otherwise.
     */
    bool TitleNameInput(std::string_view label, const std::string& currentName, TitleType type, std::function<void(std::string)> onChange);

    /**
     * A custom input field for editing a province's name.
     * @param label The label for the input field.
     * @param currentName The current name of the province.
     * @param onChange A function to be called when the name is changed.
     * @return True if the name was changed, false otherwise.
     */
    bool ProvinceNameInput(std::string_view label, const std::string& currentName, std::function<void(std::string)> onChange);
    
    /**
     * A custom input field for editing a region's name.
     * @param label The label for the input field.
     * @param currentName The current name of the region.
     * @param onChange A function to be called when the name is changed.
     * @return True if the name was changed, false otherwise.
     */
    bool RegionNameInput(std::string_view label, const std::string& currentName, std::function<void(std::string)> onChange);
}