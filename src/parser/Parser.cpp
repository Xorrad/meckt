#include "Parser.hpp"
#include <filesystem>

using namespace Parser;
using namespace Parser::Impl;

////////////////////////////////
//         Node class         //
////////////////////////////////

Node::Node() : m_Value(MakeShared<NodeHolder>()) {

}

Node::Node(const Node& node)
: m_Value((node.m_Value == nullptr) ? nullptr : node.m_Value->Copy())
{

}

Node::Node(const RawValue& value) : m_Value(MakeShared<LeafHolder>(value)) {

}

Node::Node(const std::map<Key, Node>& values) : m_Value(MakeShared<NodeHolder>(values)) {

}

ValueType Node::GetType() const {
    return m_Value->GetType();
}

bool Node::Is(ValueType type) const {
    return this->GetType() == type;
}

bool Node::IsList() const {
    ValueType t = this->GetType();
    return (t == ValueType::NUMBER_LIST)
        || (t == ValueType::BOOL_LIST)
        || (t == ValueType::STRING_LIST);

}

void Node::Push(const RawValue& value) {
    if(this->GetType() == ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::Push' on non-leaf node.");
    
    auto holder = this->GetLeafHolder();

    #define CreateList(T) holder->m_Value = std::vector<T>{std::get<T>(holder->m_Value)};

    // Check if the value to push into the list
    // is a single value or another list.
    #define PushToList(T) \
        if(value.index() < 3) { \
            std::get<std::vector<T>>(holder->m_Value).push_back(std::get<T>(value)); \
        } \
        else { \
            for(const auto& v : std::get<std::vector<T>>(value)) \
                std::get<std::vector<T>>(holder->m_Value).push_back(v); \
        } \

    switch(this->GetType()) {
        case ValueType::NUMBER:
            CreateList(double);
            PushToList(double);
            break;
        case ValueType::BOOL:
            CreateList(bool);
            PushToList(bool);
            break;
        case ValueType::STRING:
            CreateList(std::string);
            PushToList(std::string);
            break;
        case ValueType::NUMBER_LIST:
            PushToList(double);
            break;
        case ValueType::BOOL_LIST:
            PushToList(bool);
            break;
        case ValueType::STRING_LIST:
            PushToList(std::string);
            break;
        default:
            break;
    }
}

Node& Node::Get(const Key& key) {
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::Get' on leaf node.");

    // if(this->ContainsKey(key))
    //     this->Put(key, Node());

    return this->GetNodeHolder()->m_Values[key];
}

const Node& Node::Get(const Key& key) const {
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::Get' on leaf node.");
    return this->GetNodeHolder()->m_Values[key];
}

std::map<Key, Node>& Node::GetEntries() {
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::GetEntries' on leaf node.");
    return this->GetNodeHolder()->m_Values;
}

const std::map<Key, Node>& Node::GetEntries() const {
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::GetEntries' on leaf node.");
    return this->GetNodeHolder()->m_Values;
}

// std::vector<Node&> Node::GetValues() {
//     if(this->GetType() != ValueType::NODE)
//         throw std::runtime_error("error: invalid use of 'Node::GetValues' on leaf node.");
    
//     std::vector<Node&> values;
//     auto holder = this->GetNodeHolder();
//     for(auto& [key, value] : holder->m_Values)
//         values.push_back(value);
        
//     return values;
// }

std::vector<Key> Node::GetKeys() const {
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::GetKeys' on leaf node.");
    
    std::vector<Key> keys;
    auto holder = this->GetNodeHolder();
    for(const auto& [key, value] : holder->m_Values)
        keys.push_back(key);

    return keys;
}

bool Node::ContainsKey(const Key& key) const{
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::ContainsKey' on leaf node.");
    return this->GetNodeHolder()->m_Values.count(key) > 0;
}

void Node::Put(const Key& key, const Node& node) {
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::Put' on leaf node.");
    this->GetNodeHolder()->m_Values[key] = node;
}

Node::operator int() const {
    if(this->GetType() != ValueType::NUMBER)
        throw std::runtime_error("error: invalid cast from 'node' to type 'int'");
    return (int) std::get<double>(this->GetLeafHolder()->m_Value);
}

Node::operator double() const {
    if(this->GetType() != ValueType::NUMBER)
        throw std::runtime_error("error: invalid cast from 'node' to type 'double'");
    return std::get<double>(this->GetLeafHolder()->m_Value);
}

Node::operator bool() const {
    if(this->GetType() != ValueType::BOOL)
        throw std::runtime_error("error: invalid cast from 'node' to type 'bool'");
    return std::get<bool>(this->GetLeafHolder()->m_Value);
}

Node::operator std::string() const {
    if(this->GetType() != ValueType::STRING)
        throw std::runtime_error("error: invalid cast from 'node' to type 'std::string'");
    return std::get<std::string>(this->GetLeafHolder()->m_Value);
}

Node::operator std::vector<double>&() const {
    if(this->GetType() != ValueType::NUMBER_LIST)
        throw std::runtime_error("error: invalid cast from 'node' to type 'std::vector<double>&'");
    return std::get<std::vector<double>>(this->GetLeafHolder()->m_Value);
}

Node::operator std::vector<bool>&() const {
    if(this->GetType() != ValueType::BOOL_LIST)
        throw std::runtime_error("error: invalid cast from 'node' to type 'std::vector<bool>&'");
    return std::get<std::vector<bool>>(this->GetLeafHolder()->m_Value);
}

Node::operator std::vector<std::string>&() const {
    if(this->GetType() != ValueType::STRING_LIST)
        throw std::runtime_error("error: invalid cast from 'node' to type 'std::vector<std::string>&'");
    return std::get<std::vector<std::string>>(this->GetLeafHolder()->m_Value);
}

Node::operator RawValue&() const {
    if(this->GetType() == ValueType::NODE)
        throw std::runtime_error("error: invalid cast from 'node' to type 'RawValue&'");
    return this->GetLeafHolder()->m_Value;
}

Node& Node::operator=(const RawValue& value) {
    if(this->GetType() == ValueType::NODE)
        m_Value = MakeShared<LeafHolder>(value);
    else
        this->GetLeafHolder()->m_Value = value;
    return *this;
}

Node& Node::operator=(const Node& value) {
    if(this->GetType() != ValueType::NODE && value.GetType() != ValueType::NODE)
        this->GetLeafHolder()->m_Value = value.GetLeafHolder()->m_Value;
    else
        m_Value = value.m_Value->Copy();
    return *this;
}

Node& Node::operator [](const Key& key) {
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'operator[]' on leaf node.");
    return this->Get(key);
}

const Node& Node::operator [](const Key& key) const {
    if(this->GetType() != ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'operator[] const' on leaf node.");
    return this->Get(key);
}

SharedPtr<LeafHolder> Node::GetLeafHolder() {
    return std::dynamic_pointer_cast<LeafHolder>(m_Value);
}

const SharedPtr<LeafHolder> Node::GetLeafHolder() const {
    return std::dynamic_pointer_cast<LeafHolder>(m_Value);
}

SharedPtr<NodeHolder> Node::GetNodeHolder() {
    return std::dynamic_pointer_cast<NodeHolder>(m_Value);
}

const SharedPtr<NodeHolder> Node::GetNodeHolder() const {
    return std::dynamic_pointer_cast<NodeHolder>(m_Value);
}

////////////////////////////////
//      NodeHolder class      //
////////////////////////////////


NodeHolder::NodeHolder() {

}

NodeHolder::NodeHolder(const NodeHolder& n) {
    for(auto [key, value] : n.m_Values) {
        m_Values[key] = *(&value);
    }
}

NodeHolder::NodeHolder(const std::map<Key, Node>& values) {
    m_Values = values;
}

ValueType NodeHolder::GetType() const {
    return ValueType::NODE;
}

SharedPtr<AbstractValueHolder> NodeHolder::Copy() const {
    SharedPtr<NodeHolder> copy = MakeShared<NodeHolder>();
    for(const auto&[key, value] : m_Values) {
        copy->m_Values[key] = *(&value);
    }
    return copy;
}

////////////////////////////////
//      LeafHolder class      //
////////////////////////////////

LeafHolder::LeafHolder() : m_Value(0.0) {}

LeafHolder::LeafHolder(const LeafHolder& l) : m_Value(l.m_Value) {}

LeafHolder::LeafHolder(const RawValue& value) : m_Value(value) {}

ValueType LeafHolder::GetType() const {
    return (ValueType) m_Value.index();
}

SharedPtr<AbstractValueHolder> LeafHolder::Copy() const {
    SharedPtr<LeafHolder> copy = MakeShared<LeafHolder>();
    copy->m_Value = m_Value;
    return copy;
}

Node Parser::Parse(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string content = File::ReadString(file);
    file.close();

    std::deque<PToken> tokens = Parser::Lex(content);
    return Parse(tokens);
}

Node Parser::Parse(std::deque<PToken>& tokens) {
    enum ParsingState { KEY, OPERATOR, VALUE };
    ParsingState state = KEY;

    Node values;
    Key key;

    while(!tokens.empty()) {
        PToken token = tokens.front();
        tokens.pop_front();

        if(token->Is(TokenType::RIGHT_BRACE))
            break;
            
        if(token->Is(TokenType::COMMENT))
            continue;

        switch(state) {
            case KEY:
                if(token->Is(TokenType::IDENTIFIER)) {
                    key = std::get<std::string>(token->GetValue());
                    state = ParsingState::OPERATOR;
                    break;
                }
                
                if(token->Is(TokenType::NUMBER)) {
                    key = std::get<double>(token->GetValue());
                    state = ParsingState::OPERATOR;
                    break;
                }
                
                if(token->Is(TokenType::DATE)) {
                    key = std::get<Date>(token->GetValue());
                    state = ParsingState::OPERATOR;
                    break;
                }
                throw std::runtime_error(fmt::format("Unexpected token while parsing key ({}).", (int) token->GetType()));
                break;

            case OPERATOR:
                if(token->Is(TokenType::EQUAL)
                    || token->Is(TokenType::GREATER)
                    || token->Is(TokenType::GREATER_EQUAL)
                    || token->Is(TokenType::LESS)
                    || token->Is(TokenType::LESS_EQUAL)
                    // || token->Is(TokenType::TWO_DOTS)
                ) {
                    state = ParsingState::VALUE;
                }
                // throw std::runtime_error("Unexpected token while parsing operator.");
                break;
                
            case VALUE:
                tokens.push_front(token);
                Node node = ParseNode(tokens);

                if(values.ContainsKey(key)) {
                    Node& current = values[key];

                    if(!current.Is(ValueType::NODE)) {
                        current.Push((RawValue) node);
                    }

                    // TODO: handle array of nodes?
                }
                else {
                    values[key] = std::move(node);
                }
                state = ParsingState::KEY;
                break;
        }
    }

    return values;
}

Node Parser::Impl::ParseNode(std::deque<PToken>& tokens) {
    PToken token = tokens.front();
    tokens.pop_front();

    // Handle RANGE keyword by generating a list of all the numbers between A and B
    // as in: RANGE { A  B }
    if(token->Is(TokenType::IDENTIFIER) && std::get<std::string>(token->GetValue()) == "RANGE") {
        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing range.");

        // Remove LEFT_BRACE token from the list.
        token = tokens.front();
        tokens.pop_front();

        if(!token->Is(TokenType::LEFT_BRACE))
            throw std::runtime_error("error: unexpected token while parsing range.");

        return ParseRange(tokens);
    }

    // Skip LIST keyword.
    if(token->Is(TokenType::IDENTIFIER) && std::get<std::string>(token->GetValue()) == "LIST") {
        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing list.");

        // Remove LEFT_BRACE token from the list.
        token = tokens.front();
        tokens.pop_front();

        if(!token->Is(TokenType::LEFT_BRACE))
            throw std::runtime_error("error: unexpected token while parsing list.");
    }

    // Handle simple/raw values such as number, bool, string...
    else if(!token->Is(TokenType::LEFT_BRACE)) {
        return ParseRaw(token);
    }

    // Handle lists: { 1 2 3 4 5 }
    // Check if two successive tokens are of the same type.
    // So, if there isn't any operators for the second tokens,
    // it has to be a list.
    if(IsList(tokens)) {
        // The current token, which is LEFT_BRACE, won't be useful, so we can discard it.
        token = tokens.front();

        if(token->Is(TokenType::NUMBER))
            return ParseList<double>(tokens);
        
        if(token->Is(TokenType::IDENTIFIER) || token->Is(TokenType::STRING))
            return ParseList<std::string>(tokens);
    }
    
    return Parse(tokens);
    // throw std::runtime_error("error: failed to parse node value.");
}

Node Parser::Impl::ParseRaw(PToken token) {
    switch(token->GetType()) {
        case TokenType::BOOLEAN:
            return Node(std::get<bool>(token->GetValue()));
        case TokenType::DATE:
            return Node(std::get<Date>(token->GetValue()));
        case TokenType::IDENTIFIER:
            return Node(std::get<std::string>(token->GetValue()));
        case TokenType::NUMBER:
            return Node(std::get<double>(token->GetValue()));
        case TokenType::STRING:
            return Node(std::get<std::string>(token->GetValue()));
        default:
            throw std::runtime_error("error: unexpected token while parsing value.");
    }
}

Node Parser::Impl::ParseRange(std::deque<PToken>& tokens) {
    
    // First, check if the first two tokens are numbers
    // and initialize the minimum and maximum between
    // the two, as default for the range.
    if(tokens.size() < 3)
        throw std::runtime_error("error: unexpected end while parsing range.");
    
    PToken firstToken = tokens.at(0);
    PToken secondToken = tokens.at(1);
    
    tokens.pop_front();
    tokens.pop_front();

    if(!firstToken->Is(TokenType::NUMBER) || !secondToken->Is(TokenType::NUMBER))
        throw std::runtime_error("error: unexpected token while parsing range.");

    if(tokens.empty())
        throw std::runtime_error("error: unexpected end while parsing list.");

    int first = (int) std::get<double>(firstToken->GetValue());
    int second = (int) std::get<double>(secondToken->GetValue());
    int min = std::min(first, second), max = std::max(first, second);

    // Loop over the list and keep the minimum and the maximum,
    // then generate a list/vector of all the numbers in that range.
    PToken token;

    // The RIGHT_BRACE token must be removed from the list before returning.
    // The first token exists because the size of the list has been checked
    // at the beginning of the function.
    token = tokens.front();
    tokens.pop_front();

    while(!token->Is(TokenType::RIGHT_BRACE)) {
        if(!token->Is(TokenType::NUMBER))
            throw std::runtime_error("error: unexpected token while parsing range.");
        
        int n = (int) std::get<double>(token->GetValue());
        min = std::min(min, n);
        max = std::max(max, n);

        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing range.");
        
        token = tokens.front();
        tokens.pop_front();
    }

    std::vector<double> list;
    for(int i = min; i <= max; i++)
        list.push_back((double) i);

    // Use std::move to avoid copying the object and thus the array.
    return std::move(Node(list));
}

template<typename T>
Node Parser::Impl::ParseList(std::deque<PToken>& tokens) {
    std::vector<T> list;
    PToken token;
    
    // The RIGHT_BRACE token must be removed from the list before returning.
    token = tokens.front();
    tokens.pop_front();

    while(!token->Is(TokenType::RIGHT_BRACE)) {
        list.push_back(std::get<T>(token->GetValue()));

        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing list.");
        
        token = tokens.front();
        tokens.pop_front();
    }
    
    // Use std::move to avoid copying the object and thus the array.
    return std::move(Node(list));
}
    
bool Parser::Impl::IsList(std::deque<PToken>& tokens) {
    if(tokens.size() < 2)
        return false;

    PToken firstToken = tokens.at(0);
    PToken secondToken = tokens.at(1);

    #define IS_LIST_TYPE(t) (t->Is(TokenType::IDENTIFIER) || t->Is(TokenType::NUMBER) || t->Is(TokenType::STRING))

    // Check if two successive tokens are of the same type.
    // So, if there isn't any operators for the second tokens,
    // it has to be a list.

    // Or if there is only an element in the list, check if the second
    // token is a RIGHT_BRACE.
    return (secondToken->Is(firstToken->GetType()) && IS_LIST_TYPE(secondToken))
        || (secondToken->Is(TokenType::RIGHT_BRACE) && IS_LIST_TYPE(firstToken));
}

void Parser::Benchmark() {
    sf::Clock clock;

    // std::string filePath = "test_mod/map_data/default.map";
    std::string filePath = "test_mod/parser_test.txt";

    // std::string filePath = "test_mod/map_data/island_region.txt";
    // std::string filePath = "test_mod/map_data/geographical_regions/00_agot_geographical_region.txt";
    // std::string filePath = "test_mod/history/characters/00_agot_char_vale_ancestors.txt";

    std::ifstream file(filePath);

    std::string content = File::ReadString(file);
    file.close();
    
    sf::Time elapsedFile = clock.restart();

    std::deque<PToken> tokens = Parser::Lex(content);
    int tokensCount = tokens.size();

    sf::Time elapsedLexer = clock.restart();

    Node result = Parser::Parse(tokens);
    sf::Time elapsedParser = clock.getElapsedTime();

    fmt::println("file path = {}", filePath);
    fmt::println("file size = {}", String::FileSizeFormat(std::filesystem::file_size(filePath)));
    fmt::println("tokens = {}", tokensCount);
    fmt::println("entries = {}", result.GetEntries().size());
    fmt::println("elapsed file   = {}", String::DurationFormat(elapsedFile));
    fmt::println("elapsed lexer  = {}", String::DurationFormat(elapsedLexer));
    fmt::println("elapsed parser = {}", String::DurationFormat(elapsedParser));
    fmt::println("elapsed total  = {}", String::DurationFormat(elapsedFile + elapsedLexer + elapsedParser));

    // Display the result.
    fmt::println("\n------------------RESULT--------------------\n");
    fmt::println("\n{}", result);
    fmt::println("\n--------------------------------------------\n");

    ///////////////////////////
    // Test default.map file //
    ///////////////////////////
    
    if(filePath == "test_mod/map_data/default.map") {
        fmt::println("\nlakes = {}", result.Get("lakes"));
        fmt::println("\nisland_region => {}", result.Get("island_region"));
    }

    ///////////////////////////////
    // Test parser_test.txt file //
    ///////////////////////////////

    if(filePath == "test_mod/parser_test.txt") {
        // Test number list.
        fmt::println("\nlist = {}", result.Get("list"));
            
        // Test range.
        fmt::println("\nrange = {}", result.Get("range"));
        
        // Test string list.
        fmt::println("\nstring_list = {}", result.Get("string_list"));
        
        // Test for single raw values.
        fmt::println("\nidentifier => {}", result.Get("identifier"));
        fmt::println("string => {}", result.Get("string"));
        fmt::println("number => {}", result.Get("number"));
        fmt::println("bool => {}", result.Get("bool"));
        fmt::println("date => {}", result.Get("date"));

        // Test date as key
        fmt::println("1000.1.1 => {}", result.Get(Date(1000, 1, 1)));
    }
}