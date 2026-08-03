#include "Adjacency.hpp"

Adjacency::Adjacency() :
    m_FromId(0),
    m_ToId(0),
    m_Type(""),
    m_ThroughId(0),
    m_Start(sf::Vector2u(0, 0)),
    m_Stop(sf::Vector2u(0, 0)),
    m_Comment("")
{}

Adjacency::Adjacency(int fromId, int toId, std::string type, int throughId, sf::Vector2u start, sf::Vector2u stop, const std::string& comment) :
    m_FromId(fromId),
    m_ToId(toId),
    m_Type(type),
    m_ThroughId(throughId),
    m_Start(start),
    m_Stop(stop),
    m_Comment(comment)
{}

//////////////////////////////////////////////////////

std::pair<int, int> Adjacency::GetId() const {
    return std::make_pair(
        std::min(m_FromId, m_ToId),
        std::max(m_FromId, m_ToId)
    );
}

int Adjacency::GetFromId() const {
    return m_FromId;
}

int Adjacency::GetToId() const {
    return m_ToId;
}

std::string Adjacency::GetType() const {
    return m_Type;
}

int Adjacency::GetThroughId() const {
    return m_ThroughId;
}

sf::Vector2u Adjacency::GetStart() const {
    return m_Start;
}

sf::Vector2u Adjacency::GetStop() const {
    return m_Stop;
}

std::string Adjacency::GetComment() const {
    return m_Comment;
}

//////////////////////////////////////////////////////

void Adjacency::SetFromId(int fromId) {
    m_FromId = fromId;
}

void Adjacency::SetToId(int toId) {
    m_ToId = toId;
}

void Adjacency::SetType(const std::string& type) {
    m_Type = type;
}

void Adjacency::SetThroughId(int throughId) {
    m_ThroughId = throughId;
}

void Adjacency::SetStart(const sf::Vector2u& start) {
    m_Start = start;
}

void Adjacency::SetStop(const sf::Vector2u& stop) {
    m_Stop = stop;
}

void Adjacency::SetComment(const std::string& comment) {
    m_Comment = comment;
}

//////////////////////////////////////////////////////