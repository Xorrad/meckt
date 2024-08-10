#include "Parser.hpp"
#include <filesystem>

using Parser::PToken;
using Parser::TokenValue;
using Parser::TokenType;
using Parser::Token;

using Parser::PValue;
using Parser::Key;
using Parser::Value;
using Parser::Node;
using Parser::Leaf;

Node::Node() {

}

Node::Node(const Node& n) {
    m_Values = n.m_Values;
}

Node::Node(const std::map<Key, PValue>& values) {
    m_Values = values;
}

bool Node::IsLeaf() const {
    return false;
}

int Node::Count() const {
    return m_Values.size();
}

int Node::GetInt() const {
    throw std::runtime_error("undefined behaviour while calling Node::GetInt()");
}

double Node::GetDouble() const {
    throw std::runtime_error("undefined behaviour while calling Node::GetDouble()");
}

bool Node::GetBool() const {
    throw std::runtime_error("undefined behaviour while calling Node::GetBool()");
}

std::string Node::GetString() const {
    throw std::runtime_error("undefined behaviour while calling Node::GetString()");
}

PValue Node::Get(const Key& key) {
    return m_Values.at(key);
}

PValue Node::Get(const Key& key) const {
    return m_Values.at(key);
}

void Node::Set(const Key& key, int value) {
    m_Values[key] = MakeShared<Leaf>(value);
}

void Node::Set(const Key& key, double value) {
    m_Values[key] = MakeShared<Leaf>(value);
}

void Node::Set(const Key& key, bool value) {
    m_Values[key] = MakeShared<Leaf>(value);
}

void Node::Set(const Key& key, std::string value) {
    m_Values[key] = MakeShared<Leaf>(value);
}

void Node::Set(const Key& key, const PValue& value) {
    m_Values[key] = value->Copy();
}

PValue Node::operator=(int v) {
    return MakeShared<Leaf>(v);
}

PValue Node::operator=(double v) {
    return MakeShared<Leaf>(v);
}

PValue Node::operator=(bool v) {
    return MakeShared<Leaf>(v);
}

PValue Node::operator=(std::string v) {
    return MakeShared<Leaf>(v);
}

PValue Node::operator=(PValue v) {
    return v->Copy();
}

PValue Node::Copy() const {
    SharedPtr<Node> copy = MakeShared<Node>();
    for(const auto&[key, value] : m_Values) {
        copy->m_Values[key] = value->Copy();
    }
    return copy;
}

PValue Node::operator[](const Key& key) {
    return this->Get(key);
}

PValue Node::operator[](const Key& key) const {
    return this->Get(key);
}

Leaf::Leaf() {}
Leaf::Leaf(const Leaf& l) : m_Value(l.m_Value) {}
Leaf::Leaf(int v) : m_Value((double) v) {}
Leaf::Leaf(double v) : m_Value(v) {}
Leaf::Leaf(bool v) : m_Value(v) {}
Leaf::Leaf(std::string v) : m_Value(v) {}

bool Leaf::IsLeaf() const {
    return true;
}

int Leaf::Count() const {
    return 0;
}

int Leaf::GetInt() const {
    double d = std::get<double>(m_Value);
    return (int) d;
}

double Leaf::GetDouble() const {
    return std::get<double>(m_Value);
}

bool Leaf::GetBool() const {
    return std::get<bool>(m_Value);
}

std::string Leaf::GetString() const {
    return std::get<std::string>(m_Value);
}

PValue Leaf::Get(const Key& key) {
    throw std::runtime_error("undefined behaviour while calling Leaf::Get()");
}

PValue Leaf::Get(const Key& key) const {
    throw std::runtime_error("undefined behaviour while calling Leaf::Get()");
}

void Leaf::Set(const Key& key, const PValue& value) {
    throw std::runtime_error("undefined behaviour while calling Leaf::Set()");
}

void Leaf::Set(const Key& key, int value) {
    throw std::runtime_error("undefined behaviour while calling Leaf::Set()");
}

void Leaf::Set(const Key& key, double value) {
    throw std::runtime_error("undefined behaviour while calling Leaf::Set()");
}

void Leaf::Set(const Key& key, bool value) {
    throw std::runtime_error("undefined behaviour while calling Leaf::Set()");
}

void Leaf::Set(const Key& key, std::string value) {
    throw std::runtime_error("undefined behaviour while calling Leaf::Set()");
}

PValue Leaf::operator=(int v) {
    m_Value = (double) v;
    return MakeShared<Leaf>(this);
}

PValue Leaf::operator=(double v) {
    m_Value = v;
    return MakeShared<Leaf>(this);
}

PValue Leaf::operator=(bool v) {
    m_Value = v;
    return MakeShared<Leaf>(this);
}

PValue Leaf::operator=(std::string v) {
    m_Value = v;
    return MakeShared<Leaf>(this);
}

PValue Leaf::operator=(PValue v) {
    return v->Copy();
}

PValue Leaf::operator[](const Key& key) {
    throw std::runtime_error("undefined behaviour while calling Leaf::operator[]()");
}

PValue Leaf::operator[](const Key& key) const {
    throw std::runtime_error("undefined behaviour while calling Leaf::operator[]()");
}

PValue Leaf::Copy() const {
    SharedPtr<Leaf> copy = MakeShared<Leaf>();
    copy->m_Value = m_Value;
    return copy;
}

PValue Parser::Parse(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string content = File::ReadString(file);
    file.close();

    std::queue<PToken> tokens = Parser::Lex(content);
    return Parse(tokens);
}

PValue Parser::Parse(std::queue<PToken>& tokens) {
    enum ParsingState { KEY, OPERATOR, VALUE };
    ParsingState state = KEY;

    PValue values = MakeShared<Node>();
    Key key;

    while(!tokens.empty()) {
        PToken token = tokens.front();
        tokens.pop();

        // fmt::println("token {}", (int) token->GetType());
        // fmt::println("state {}", (int) state);

        if(token->Is(TokenType::RIGHT_BRACE))
            break;

        switch(state) {
            case KEY:
                if(token->Is(TokenType::IDENTIFIER)) {
                    // fmt::println("switch state to OPERATOR");
                    key = std::get<std::string>(token->GetValue());
                    state = ParsingState::OPERATOR;
                    break;
                }
                
                if(token->Is(TokenType::NUMBER)) {
                    key = std::get<double>(token->GetValue());
                    state = ParsingState::OPERATOR;
                    // fmt::println("switch state to OPERATOR");
                    break;
                }
                throw std::runtime_error("Unexpected token while parsing key.");
                break;

            case OPERATOR:
                if(token->Is(TokenType::EQUAL)) {
                    state = ParsingState::VALUE;
                    // fmt::println("switch state to VALUE");
                }
                // throw std::runtime_error("Unexpected token while parsing operator.");
                break;
                
            case VALUE:
                switch(token->GetType()) {
                    case TokenType::LEFT_BRACE:
                        values->Set(key, Parse(tokens));
                        break;
                    case TokenType::BOOLEAN:
                        values->Set(key, std::get<bool>(token->GetValue()));
                        break;
                    case TokenType::DATE:
                        values->Set(key, std::get<std::string>(token->GetValue()));
                        break;
                    case TokenType::IDENTIFIER:
                        // fmt::println("value is IDENTIFIER {} {}", std::get<std::string>(key), std::get<std::string>(token->GetValue()));
                        values->Set(key, std::get<std::string>(token->GetValue()));
                        // fmt::println("after value set");
                        break;
                    case TokenType::NUMBER:
                        // fmt::println("value is NUMBER");
                        values->Set(key, std::get<double>(token->GetValue()));
                        break;
                    case TokenType::STRING:
                        values->Set(key, std::get<std::string>(token->GetValue()));
                        break;
                    default:
                        throw std::runtime_error("Unexpected token while parsing value.");
                        break;
                }
                state = ParsingState::KEY;
                // fmt::println("switch state to KEY");
                break;
        }
    }

    return values;
}

void Parser::Benchmark() {
    sf::Clock clock;

    // std::string filePath = "test_mod/map_data/default.map";
    // std::string filePath = "test_mod/map_data/island_region.txt";
    // std::string filePath = "test_mod/map_data/geographical_regions/00_agot_geographical_region.txt";
    // std::string filePath = "test_mod/history/characters/00_agot_char_vale_ancestors.txt";
    std::string filePath = "test_mod/parser_test.txt";

    std::ifstream file(filePath);

    std::string content = File::ReadString(file);
    file.close();
    
    sf::Time elapsedFile = clock.restart();

    std::queue<PToken> tokens = Parser::Lex(content);

    sf::Time elapsedLexer = clock.restart();

    PValue entries = Parser::Parse(tokens);
    sf::Time elapsedParser = clock.getElapsedTime();

    fmt::println("file path = {}", filePath);
    fmt::println("file size = {}", String::FileSizeFormat(std::filesystem::file_size(filePath)));
    fmt::println("tokens = {}", tokens.size());
    fmt::println("entries = {}", entries->Count());
    fmt::println("elapsed file   = {}", String::DurationFormat(elapsedFile));
    fmt::println("elapsed lexer  = {}", String::DurationFormat(elapsedLexer));
    fmt::println("elapsed parser = {}", String::DurationFormat(elapsedParser));
    fmt::println("elapsed total  = {}", String::DurationFormat(elapsedLexer + elapsedParser));
    
    fmt::println("\n");
    // fmt::println("{}", entries->Get("test")->GetString());
}