#pragma once

class Adjacency {
public:
    
    /**
     * @brief Constructs a new adjacency with default values.
     */
    Adjacency();

    /**
     * @brief Constructs a new adjacency with the specified name.
     * @param fromId The ID of the province from which the adjacency starts.
     * @param toId The ID of the province to which the adjacency leads.
     * @param type The type of the adjacency (e.g. `sea`, `river_large`).
     * @param throughId The ID of the province through which the adjacency passes.
     * @param start The starting position of the adjacency.
     * @param stop The stopping position of the adjacency.
     * @param comment The comment for the adjacency.
     */
    Adjacency(int fromId, int toId, std::string type, int throughId, sf::Vector2u start, sf::Vector2u stop, const std::string& comment);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the shared Id of the adjacency as a pair.
     * @return A pair of the adjacency IDs sorted from smaller to bigger.
     */
    std::pair<int, int> GetId() const;

    /**
     * @brief Gets the ID of the province from which the adjacency starts.
     * @return The ID of the starting province.
     */
    int GetFromId() const;

    /**
     * @brief Gets the ID of the province to which the adjacency leads.
     * @return The ID of the destination province.
     */
    int GetToId() const;

    /**
     * @brief Gets the type of the adjacency.
     * @example `sea`, `river_large`
     * @return The type of the adjacency.
     */
    std::string GetType() const;

    /**
     * @brief Gets the ID of the province through which the adjacency passes.
     * @return The ID of the province through which the adjacency passes.
     */
    int GetThroughId() const;

    /**
     * @brief Gets the starting position of the adjacency.
     * @return The starting position of the adjacency.
     */
    sf::Vector2u GetStart() const;

    /**
     * @brief Gets the stopping position of the adjacency.
     * @return The stopping position of the adjacency.
     */
    sf::Vector2u GetStop() const;

    /**
     * @brief Gets the comment for the adjacency.
     * @return The comment for the adjacency.
     */
    std::string GetComment() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the ID of the province from which the adjacency starts.
     * @param fromId The new ID of the starting province.
     */
    void SetFromId(int fromId);

    /**
     * @brief Sets the ID of the province to which the adjacency leads.
     * @param toId The new ID of the destination province.
     */
    void SetToId(int toId);

    /**
     * @brief Sets the type of the adjacency.
     * @param type The new type of the adjacency.
     */
    void SetType(const std::string& type);

    /**
     * @brief Sets the ID of the province through which the adjacency passes.
     * @param throughId The new ID of the province through which the adjacency passes.
     */
    void SetThroughId(int throughId);

    /**
     * @brief Sets the starting position of the adjacency.
     * @param start The new starting position of the adjacency.
     */
    void SetStart(const sf::Vector2u& start);

    /**
     * @brief Sets the stopping position of the adjacency.
     * @param stop The new stopping position of the adjacency.
     */
    void SetStop(const sf::Vector2u& stop);

    /**
     * @brief Sets the comment for the adjacency.
     * @param comment The new comment for the adjacency.
     */
    void SetComment(const std::string& comment);

    //////////////////////////////////////////////////////

private:
    int m_FromId;
    int m_ToId;
    std::string m_Type;
    int m_ThroughId;
    sf::Vector2u m_Start;
    sf::Vector2u m_Stop;
    std::string m_Comment;
};