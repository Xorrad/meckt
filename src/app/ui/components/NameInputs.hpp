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
}