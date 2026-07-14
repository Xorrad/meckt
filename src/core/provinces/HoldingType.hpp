#pragma once

class HoldingType {
public:
    
    /**
     * @brief Constructs a new holding type with default values.
     */
    HoldingType();

    /**
     * @brief Constructs a new holding type with the specified name.
     * @param name The name of the holding type.
     */
    HoldingType(const std::string& name);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the name of the holding type.
     * @return The name of the holding type.
     */
    std::string GetName() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the name of the holding type.
     * @param name The new name of the holding type.
     */
    void SetName(const std::string& name);

    //////////////////////////////////////////////////////

private:
    std::string m_Name;
};