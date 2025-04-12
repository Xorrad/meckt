#include "Parser.hpp"
#include <filesystem>

using namespace Parser;
using namespace Parser::Impl;

////////////////////////////////
//        Object class        //
////////////////////////////////

Object::Object() :
    m_Value(MakeShared<ObjectHolder>())
{}

Object::Object(const Object& object) :
    m_Value((object.m_Value == nullptr) ? nullptr : object.m_Value->Copy())
{}

Object::Object(const Scalar& value) : 
    m_Value(MakeShared<ScalarHolder>(value))
{}

Object::Object(const Array& value) : 
    m_Value(MakeShared<ArrayHolder>(value))
{}

Object::Object(const std::vector<SharedPtr<Object>>& value) : 
    m_Value(MakeShared<ArrayHolder>(value))
{}

Object::Object(const sf::Color& color) : 
    m_Value(MakeShared<ArrayHolder>(std::vector<int>{(int) color.r, (int) color.g, (int) color.b}))
{}

ObjectType Object::GetType() const {
    return m_Value->GetType();
}

ObjectType Object::GetArrayType() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::GetArrayType' on scalar or object.");
    return this->GetArrayHolder()->GetArrayType();
}

bool Object::Is(ObjectType type) const {
    return this->GetType() == type;
}

void Object::ConvertToArray() {
    if(this->Is(ObjectType::ARRAY)) {
        return;
    }
    
    if(this->Is(ObjectType::OBJECT)) {
        // Create a new object and add the values to it
        // then convert the current object to an array
        // with the new object as the only element.
        auto values = this->GetObjectHolder()->m_Values;
        SharedPtr<Object> object = MakeShared<Object>();
        object->GetObjectHolder()->m_Values = values;
        m_Value = MakeShared<ArrayHolder>(std::vector<SharedPtr<Object>>{object});
    }
    else {
        Scalar value = this->GetScalarHolder()->m_Value;
        #define AS_ARRAY(T) m_Value = MakeShared<ArrayHolder>(std::vector<T>{std::get<T>(value)}); break
        switch((ObjectType) value.index()) {
            case ObjectType::INT: AS_ARRAY(int);
            case ObjectType::DECIMAL: AS_ARRAY(double);
            case ObjectType::BOOL: AS_ARRAY(bool);
            case ObjectType::STRING: AS_ARRAY(std::string);
            case ObjectType::DATE: AS_ARRAY(Date);
            case ObjectType::SCOPED_STRING: AS_ARRAY(ScopedString);
            default: break;
        }
    }
}

void Object::Push(const SharedPtr<Object>& object) {
    this->Push(std::vector<SharedPtr<Object>>{object});
}

void Object::Push(const Scalar& scalar) {
    #define PushAsArray(T) this->Push(std::vector<T>{std::get<T>(scalar)}); break

    switch((ObjectType) scalar.index()) {
        case ObjectType::INT: PushAsArray(int);
        case ObjectType::DECIMAL: PushAsArray(double);
        case ObjectType::BOOL: PushAsArray(bool);
        case ObjectType::STRING: PushAsArray(std::string);
        case ObjectType::DATE: PushAsArray(Date);
        case ObjectType::SCOPED_STRING: PushAsArray(ScopedString);
        default: break;
    }
}

void Object::Push(const Array& array) {
    if(!this->Is(ObjectType::ARRAY)) {
        this->ConvertToArray();
    }

    auto holder = this->GetArrayHolder();

    #define MergeArrays(T) { \
        if((int) holder->GetArrayType() != array.index()) \
            throw std::runtime_error(fmt::format(FMT_COMPILE("error: cannot merge {} array into {} array."), #T, (int) holder->GetArrayType())); \
        auto& target = std::get<std::vector<T>>(holder->m_Values); \
        target.reserve(target.size() + std::get<std::vector<T>>(array).size()); \
        for(const auto& v : std::get<std::vector<T>>(array)) \
            target.push_back(v); \
    } break
    
    // TODO: handle array of arrays.
    switch((ObjectType) array.index()) {
        case ObjectType::INT: MergeArrays(int);
        case ObjectType::DECIMAL: MergeArrays(double);
        case ObjectType::BOOL: MergeArrays(bool);
        case ObjectType::STRING: MergeArrays(std::string);
        case ObjectType::DATE: MergeArrays(Date);
        case ObjectType::SCOPED_STRING: MergeArrays(ScopedString);
        case ObjectType::OBJECT: MergeArrays(SharedPtr<Object>);
        default: break;
    } 
}

void Object::Merge(const SharedPtr<Object>& object) {
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::Merge' on scalar.");
    if(!object->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::Merge' with scalar.");

    for(auto& [key, pair] : object->GetObjectHolder()->m_Values) {
        this->Put(key, pair.second, pair.first);
    }
}

template <typename T>
T Object::Get(const Scalar& key) const {
    if(this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::Get' on array.");
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::Get' on scalar.");
    return (T) (*this->GetObjectHolder()->m_Values[key].second);
}

template <>
SharedPtr<Object> Object::Get<SharedPtr<Object>>(const Scalar& key) const {
    if(this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::Get' on array.");
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::Get' on scalar.");
    auto it = this->GetObjectHolder()->m_Values.find(key);
    if(it == this->GetObjectHolder()->m_Values.end())
        return nullptr;
    return it->second.second;
}

template int Object::Get<int>(const Scalar&) const;
template double Object::Get<double>(const Scalar&) const;
template bool Object::Get<bool>(const Scalar&) const;
template std::string Object::Get<std::string>(const Scalar&) const;
template Date Object::Get<Date>(const Scalar&) const;
template ScopedString Object::Get<ScopedString>(const Scalar&) const;
template Scalar Object::Get<Scalar>(const Scalar&) const;
template sf::Color Object::Get<sf::Color>(const Scalar&) const;

template <typename T>
T Object::Get(const Scalar& key, T defaultValue) const {
    if(this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::Get' on array.");
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::Get' on scalar.");
    auto it = this->GetObjectHolder()->m_Values.find(key);
    if(it == this->GetObjectHolder()->m_Values.end())
        return defaultValue;
    return (T) (*(it->second.second));
}

template <>
SharedPtr<Object> Object::Get<SharedPtr<Object>>(const Scalar& key, SharedPtr<Object> defaultValue) const {
    if(this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::Get' on array.");
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::Get' on scalar.");
    auto it = this->GetObjectHolder()->m_Values.find(key);
    if(it == this->GetObjectHolder()->m_Values.end())
        return defaultValue;
    return this->GetObjectHolder()->m_Values[key].second;
}

template int Object::Get<int>(const Scalar&, int) const;
template double Object::Get<double>(const Scalar&, double) const;
template bool Object::Get<bool>(const Scalar&, bool) const;
template std::string Object::Get<std::string>(const Scalar&, std::string) const;
template Date Object::Get<Date>(const Scalar&, Date) const;
template ScopedString Object::Get<ScopedString>(const Scalar&, ScopedString) const;
template SharedPtr<Object> Object::Get<SharedPtr<Object>>(const Scalar&, SharedPtr<Object>) const;
template Scalar Object::Get<Scalar>(const Scalar&, Scalar) const;
template sf::Color Object::Get<sf::Color>(const Scalar&, sf::Color) const;

template <typename T>
std::vector<T> Object::GetArray(const Scalar& key) const {
    if(this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::GetArray' on array.");
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::GetArray' on scalar.");
    auto it = this->GetObjectHolder()->m_Values.find(key);
    if(!it->second.second->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::GetArray' on scalar or object.");
    return std::get<std::vector<T>>(it->second.second->GetArrayHolder()->m_Values);
}

template std::vector<int> Object::GetArray<int>(const Scalar&) const;
template std::vector<double> Object::GetArray<double>(const Scalar&) const;
template std::vector<bool> Object::GetArray<bool>(const Scalar&) const;
template std::vector<std::string> Object::GetArray<std::string>(const Scalar&) const;
template std::vector<Date> Object::GetArray<Date>(const Scalar&) const;
template std::vector<ScopedString> Object::GetArray<ScopedString>(const Scalar&) const;
template std::vector<SharedPtr<Object>> Object::GetArray<SharedPtr<Object>>(const Scalar&) const;

template <typename T>
std::vector<T> Object::GetArray(const Scalar& key, std::vector<T> defaultValue) const {
    if(this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::GetArray' on array.");
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::GetArray' on scalar.");
    auto it = this->GetObjectHolder()->m_Values.find(key);
    if(it == this->GetObjectHolder()->m_Values.end())
        return defaultValue;
    if(!it->second.second->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::GetArray' on scalar or object.");
    return std::get<std::vector<T>>(it->second.second->GetArrayHolder()->m_Values);
}

template std::vector<int> Object::GetArray<int>(const Scalar&, std::vector<int>) const;
template std::vector<double> Object::GetArray<double>(const Scalar&, std::vector<double>) const;
template std::vector<bool> Object::GetArray<bool>(const Scalar&, std::vector<bool>) const;
template std::vector<std::string> Object::GetArray<std::string>(const Scalar&, std::vector<std::string>) const;
template std::vector<Date> Object::GetArray<Date>(const Scalar&, std::vector<Date>) const;
template std::vector<ScopedString> Object::GetArray<ScopedString>(const Scalar&, std::vector<ScopedString>) const;
template std::vector<SharedPtr<Object>> Object::GetArray<SharedPtr<Object>>(const Scalar&, std::vector<SharedPtr<Object>>) const;

SharedPtr<Object> Object::GetObject(const Scalar& key) const {
    return this->Get<SharedPtr<Object>>(key);
}

Operator Object::GetOperator(const Scalar& key) {
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::GetOperator' on scalar or array.");
    return this->GetObjectHolder()->m_Values[key].first;
}

OrderedMap<Scalar, std::pair<Operator, SharedPtr<Object>>>& Object::GetEntries() {
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::GetEntries' on scalar or array.");
    return this->GetObjectHolder()->m_Values;
}

const OrderedMap<Scalar, std::pair<Operator, SharedPtr<Object>>>& Object::GetEntries() const {
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::GetEntries' on scalar or array.");
    return this->GetObjectHolder()->m_Values;
}

std::vector<Scalar> Object::GetKeys() const {
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::GetKeys' on scalar or array.");
    return this->GetObjectHolder()->m_Values.keys();
}

bool Object::ContainsKey(const Scalar& key) const{
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::ContainsKey' on scalar or array.");
    return this->GetObjectHolder()->m_Values.contains(key);
}

void Object::Put(const Scalar& key, const SharedPtr<Object>& value, Operator op) {
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::Put' on scalar or array.");
    this->GetObjectHolder()->m_Values.insert(key, std::make_pair(op, value));
}

void Object::Put(const Scalar& key, const Scalar& value, Operator op) {
    this->Put(key, MakeShared<Object>(value), op);
}

void Object::Put(const Scalar& key, const Array& value, Operator op) {
    this->Put(key, MakeShared<Object>(value), op);
}

void Object::Put(const Scalar& key, const sf::Color& value, Operator op) {
    this->Put(key, MakeShared<Object>(value), op);
}

SharedPtr<Object> Object::Remove(const Scalar& key) {
    if(!this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid use of 'Object::Remove' on scalar or array.");
    SharedPtr<Object> value = std::move(this->GetObjectHolder()->m_Values[key].second);
    this->GetObjectHolder()->m_Values.erase(key);
    return value;
}

Scalar& Object::AsScalar() {
    if(this->Is(ObjectType::OBJECT) || this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::AsScalar' on object or array.");
    return this->GetScalarHolder()->m_Value;
}

Scalar Object::AsScalar() const {
    if(this->Is(ObjectType::OBJECT) || this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::AsScalar' on object or array.");
    return this->GetScalarHolder()->m_Value;
}

Object::operator int() const {
    if(!this->Is(ObjectType::INT))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'int'");
    return std::get<int>(this->GetScalarHolder()->m_Value);
}

Object::operator double() const {
    if(!this->Is(ObjectType::DECIMAL))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'decimal'");
    return std::get<double>(this->GetScalarHolder()->m_Value);
}

Object::operator bool() const {
    if(!this->Is(ObjectType::BOOL))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'bool'");
    return std::get<bool>(this->GetScalarHolder()->m_Value);
}

Object::operator std::string() const {
    if(!this->Is(ObjectType::STRING))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::string'");
    return std::get<std::string>(this->GetScalarHolder()->m_Value);
}

Object::operator Date() const {
    if(!this->Is(ObjectType::DATE))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'Date'");
    return std::get<Date>(this->GetScalarHolder()->m_Value);
}

Object::operator ScopedString() const {
    if(!this->Is(ObjectType::SCOPED_STRING))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'ScopedString'");
    return std::get<ScopedString>(this->GetScalarHolder()->m_Value);
}

Object::operator Scalar() const {
    if(this->Is(ObjectType::ARRAY) || this->Is(ObjectType::OBJECT))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'Scalar'");
    return this->GetScalarHolder()->m_Value;
}

Array& Object::AsArray() {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::AsArray' on scalar or object.");
    return this->GetArrayHolder()->m_Values;
}

const Array& Object::AsArray() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid use of 'Object::AsArray' on scalar or object.");
    return this->GetArrayHolder()->m_Values;
}

Object::operator std::vector<int>&() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<int>&'");
    if(this->GetArrayHolder()->GetArrayType() != ObjectType::INT)
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<int>&'");
    return std::get<std::vector<int>>(this->GetArrayHolder()->m_Values);
}

Object::operator std::vector<double>&() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<double>&'");
    if(this->GetArrayHolder()->GetArrayType() != ObjectType::DECIMAL)
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<double>&'");
    return std::get<std::vector<double>>(this->GetArrayHolder()->m_Values);
}

Object::operator std::vector<bool>&() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<bool>&'");
    if(this->GetArrayHolder()->GetArrayType() != ObjectType::BOOL)
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<bool>&'");
    return std::get<std::vector<bool>>(this->GetArrayHolder()->m_Values);
}

Object::operator std::vector<std::string>&() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<std::string>&'");
    if(this->GetArrayHolder()->GetArrayType() != ObjectType::STRING)
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<std::string>&'");
    return std::get<std::vector<std::string>>(this->GetArrayHolder()->m_Values);
}

Object::operator std::vector<Date>&() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<Date>&'");
    if(this->GetArrayHolder()->GetArrayType() != ObjectType::DATE)
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<Date>&'");
    return std::get<std::vector<Date>>(this->GetArrayHolder()->m_Values);
}

Object::operator std::vector<ScopedString>&() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<ScopedString>&'");
    if(this->GetArrayHolder()->GetArrayType() != ObjectType::SCOPED_STRING)
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<ScopedString>&'");
    return std::get<std::vector<ScopedString>>(this->GetArrayHolder()->m_Values);
}

Object::operator std::vector<SharedPtr<Object>>&() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<SharedPtr<Object>>&'");
    if(this->GetArrayHolder()->GetArrayType() != ObjectType::OBJECT)
        throw std::runtime_error("error: invalid cast from 'Object' to type 'std::vector<SharedPtr<Object>>&'");
    return std::get<std::vector<SharedPtr<Object>>>(this->GetArrayHolder()->m_Values);
}

Object::operator Array&() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'Array&'");
    return this->GetArrayHolder()->m_Values;
}

Object::operator sf::Color() const {
    if(!this->Is(ObjectType::ARRAY))
        throw std::runtime_error("error: invalid cast from 'Object' to type 'sf::Color'");

    if(this->GetArrayHolder()->GetArrayType() == ObjectType::INT) {
        std::vector<int> values = std::get<std::vector<int>>(this->GetArrayHolder()->m_Values);
        if(values.size() < 3)
            throw std::runtime_error("error: invalid cast from 'Object' to type 'sf::Color'");    
        return sf::Color(values[0], values[1], values[2]);
    }
    else if(this->GetArrayHolder()->GetArrayType() == ObjectType::DECIMAL) {
        std::vector<double> values = std::get<std::vector<double>>(this->GetArrayHolder()->m_Values);
    
        if(values.size() < 3)
            throw std::runtime_error("error: invalid cast from 'Object' to type 'sf::Color'");

        // Check if the numbers are float between 0 and 1.
        // If so, then parse the list as an HSV color code.
        // TODO: Fix issues where it is parsed as HSV for low RGB values.
        if((values[0] >= 0.0 && values[0] <= 1.0)
        && (values[1] >= 0.0 && values[1] <= 1.0)
        && (values[2] >= 0.0 && values[2] <= 1.0)) {
            float h = std::min(values[0] * 360, 359.99);
            float s = std::min(values[1], 0.99);
            float v = std::min(values[2], 0.99);
            return sf::HSVColor(h, s, v).toRgb();
        }

        return sf::Color((int) values[0], (int) values[1], (int) values[2]);
    }
    throw std::runtime_error("error: invalid cast from 'Object' to type 'sf::Color'");
}

Object& Object::operator=(const Scalar& value) {
    if(this->Is(ObjectType::OBJECT) || this->Is(ObjectType::ARRAY))
        m_Value = MakeShared<ScalarHolder>(value);
    else
        this->GetScalarHolder()->m_Value = value;
    return *this;
}

Object& Object::operator=(const Object& value) {
    m_Value = value.GetHolder()->Copy();
    return *this;
}

SharedPtr<AbstractHolder> Object::GetHolder() {
    return m_Value;
}

const SharedPtr<AbstractHolder> Object::GetHolder() const {
    return m_Value;
}

SharedPtr<ScalarHolder> Object::GetScalarHolder() {
    return CastSharedPtr<ScalarHolder>(m_Value);
}

const SharedPtr<ScalarHolder> Object::GetScalarHolder() const {
    return CastSharedPtr<ScalarHolder>(m_Value);
}

SharedPtr<ArrayHolder> Object::GetArrayHolder() {
    return CastSharedPtr<ArrayHolder>(m_Value);
}

const SharedPtr<ArrayHolder> Object::GetArrayHolder() const {
    return CastSharedPtr<ArrayHolder>(m_Value);
}

SharedPtr<ObjectHolder> Object::GetObjectHolder() {
    return CastSharedPtr<ObjectHolder>(m_Value);
}

const SharedPtr<ObjectHolder> Object::GetObjectHolder() const {
    return CastSharedPtr<ObjectHolder>(m_Value);
}

////////////////////////////////
//    ScalarHolder class      //
////////////////////////////////

ScalarHolder::ScalarHolder() : m_Value(0.0) {}

ScalarHolder::ScalarHolder(const ScalarHolder& holder) : m_Value(holder.m_Value) {}

ScalarHolder::ScalarHolder(const Scalar& value) : m_Value(value) {}

ObjectType ScalarHolder::GetType() const {
    return (ObjectType) m_Value.index();
}

SharedPtr<AbstractHolder> ScalarHolder::Copy() const {
    SharedPtr<ScalarHolder> copy = MakeShared<ScalarHolder>();
    copy->m_Value = m_Value;
    return copy;
}

////////////////////////////////
//     ArrayHolder class      //
////////////////////////////////

ArrayHolder::ArrayHolder() : m_Values(std::vector<double>{})
{}

ArrayHolder::ArrayHolder(const ArrayHolder& holder) : m_Values(holder.m_Values)
{}

ArrayHolder::ArrayHolder(const Array& value) : m_Values(value)
{}

ObjectType ArrayHolder::GetType() const {
    return ObjectType::ARRAY;
}

SharedPtr<AbstractHolder> ArrayHolder::Copy() const {
    if(this->GetArrayType() != ObjectType::OBJECT) {
        SharedPtr<ArrayHolder> copy = MakeShared<ArrayHolder>();
        copy->m_Values = m_Values;
        return copy;
    }
    const std::vector<SharedPtr<Object>>& objects = std::get<std::vector<SharedPtr<Object>>>(m_Values);
    std::vector<SharedPtr<Object>> copyValues = std::vector<SharedPtr<Object>>(objects.size());
    for(const auto& object : objects) {
        copyValues.push_back(MakeShared<Object>(*object));
    }
    SharedPtr<ArrayHolder> copy = MakeShared<ArrayHolder>(copyValues);
    return copy;
}

ObjectType ArrayHolder::GetArrayType() const {
    return (ObjectType) m_Values.index();
}

////////////////////////////////
//     ObjectHolder class     //
////////////////////////////////


ObjectHolder::ObjectHolder()
{}

ObjectHolder::ObjectHolder(const ObjectHolder& holder) {
    for(auto [key, pair] : holder.m_Values) {
        m_Values[key] = std::make_pair(pair.first, MakeShared<Object>(*pair.second));
    }
}

ObjectType ObjectHolder::GetType() const {
    return ObjectType::OBJECT;
}

SharedPtr<AbstractHolder> ObjectHolder::Copy() const {
    SharedPtr<ObjectHolder> copy = MakeShared<ObjectHolder>();
    for(const auto&[key, pair] : m_Values) {
        copy->m_Values[key] = std::make_pair(pair.first, MakeShared<Object>(*pair.second));
    }
    return copy;
}

////////////////////////////////
//      Global functions      //
////////////////////////////////


SharedPtr<Object> Parser::ParseFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string content = File::ReadString(file);
    file.close();
    return Parse(content);
}

SharedPtr<Object> Parser::ParseFile(std::ifstream& file) {
    std::string content = File::ReadString(file);
    return Parse(content);
}

SharedPtr<Object> Parser::Parse(const std::string& content) {
    std::deque<PToken> tokens = Parser::Lex(content);
    SharedPtr<Object> object = Parse(tokens);
    return object;
}

SharedPtr<Object> Parser::Parse(std::deque<PToken>& tokens, uint depth) {
    enum ParsingState { KEY, OPERATOR, VALUE };
    ParsingState state = KEY;

    SharedPtr<Object> values = MakeShared<Object>();
    Scalar key;
    Operator op = Operator::EQUAL;

    while(!tokens.empty()) {
        PToken token = tokens.front();
        tokens.pop_front();

        if(token->Is(TokenType::RIGHT_BRACE)) {
            return values;
        }

        // Comments are discarded in the lexer (until they are actually used).
        // if(token->Is(TokenType::COMMENT))
        //     continue;

        switch(state) {
            case KEY:
                try {
                    key = ParseScalar(token, tokens)->AsScalar();
                    state = ParsingState::OPERATOR;
                }
                catch(std::exception& e) {
                    throw std::runtime_error(fmt::format("{}\nUnexpected token while parsing key (type={}).", e.what(), (int) token->GetType()));
                }
                break;

            case OPERATOR:
                if(token->Is(TokenType::EQUAL)
                    || token->Is(TokenType::GREATER)
                    || token->Is(TokenType::GREATER_EQUAL)
                    || token->Is(TokenType::LESS)
                    || token->Is(TokenType::LESS_EQUAL)
                    || token->Is(TokenType::NOT_EQUAL)
                    || token->Is(TokenType::EXIST)
                ) {
                    state = ParsingState::VALUE;
                    op = (Operator)(((int) token->GetType()) - 3);
                    break;
                }
                else {
                    LOG_ERROR("Unexpected token while parsing operator (key={}, type={}). Using '=' operator by default.", key, (int) token->GetType());
                    state = ParsingState::VALUE;
                    op = Operator::EQUAL;
                    tokens.emplace_front(token);
                    break;
                }
                throw std::runtime_error(fmt::format("Unexpected token while parsing operator (key={}, type={}).", key, (int) token->GetType()));
                break;
                
            case VALUE:
                tokens.push_front(token);
                SharedPtr<Object> object;
                try {
                    object = ParseObject(tokens);
                }
                catch (std::exception& e) {
                    throw std::runtime_error(fmt::format("{}\nFailed to parse value for key {}", e.what(), key));
                }

                if(values->ContainsKey(key)) {
                    SharedPtr<Object> current = values->GetObject(key);

                    if(current->Is(ObjectType::OBJECT) && object->Is(ObjectType::OBJECT)) {
                        current->Merge(object);
                    }
                    else if(object->Is(ObjectType::OBJECT)) {
                        current->Push(object);
                    }
                    else if(object->Is(ObjectType::ARRAY)) {
                        current->Push(object->AsArray());
                    }
                    else {
                        current->Push(object->AsScalar());
                    }
                }
                else {
                    values->Put(key, object, op);
                }
                state = ParsingState::KEY;
                break;
        }
    }

    // Only the root (first of Parse) can be defined without being
    // enclosed by curly brackets.
    if(depth > 0)
        throw std::runtime_error("error: missing closing curly bracket ('}').");
    
    if(state == ParsingState::OPERATOR)
        throw std::runtime_error(fmt::format("error: unexpected end after key {}.", key));
    if(state == ParsingState::VALUE)
        throw std::runtime_error(fmt::format("error: unexpected end after operator {}.", op));

    return values;
}

SharedPtr<Object> Parser::Impl::ParseObject(std::deque<PToken>& tokens) {
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

    // Skip hsv keyword
    else if(token->Is(TokenType::IDENTIFIER) && (std::get<std::string>(token->GetValue()) == "hsv" || std::get<std::string>(token->GetValue()) == "rgb")
    && !tokens.empty() && tokens.front()->Is(TokenType::LEFT_BRACE)) {
        // Remove LEFT_BRACE token from the list.
        token = tokens.front();
        tokens.pop_front();
    }

    // Handle scalars (int, decimal, bool, string, Date, ScopedString)
    else if(!token->Is(TokenType::LEFT_BRACE)) {
        return ParseScalar(token, tokens);
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
        
        if(token->Is(TokenType::BOOLEAN))
            return ParseList<bool>(tokens);
        
        if(token->Is(TokenType::IDENTIFIER) || token->Is(TokenType::STRING))
            return ParseList<std::string>(tokens);
            
        if(token->Is(TokenType::LEFT_BRACE))
            return ParseList<SharedPtr<Object>>(tokens);
    }
    
    return Parse(tokens, 1);
    // throw std::runtime_error("error: failed to parse node value.");
}

SharedPtr<Object> Parser::Impl::ParseScalar(PToken token, std::deque<PToken>& tokens) {
    switch(token->GetType()) {
        case TokenType::BOOLEAN:
            return MakeShared<Object>(std::get<bool>(token->GetValue()));
        case TokenType::DATE:
            return MakeShared<Object>(std::get<Date>(token->GetValue()));
        case TokenType::IDENTIFIER:
            return ParseString(token, tokens);
        case TokenType::NUMBER:
            // double intpart;
            // if(modf(std::get<double>(token->GetValue()), &intpart) == 0.0)
            //     return MakeShared<Object>((int) intpart);
            return MakeShared<Object>(std::get<double>(token->GetValue()));
        case TokenType::STRING:
            return MakeShared<Object>(std::get<std::string>(token->GetValue()));
        default:
            throw std::runtime_error("error: unexpected token while parsing scalar.");
    }
}

SharedPtr<Object> Parser::Impl::ParseString(PToken token, std::deque<PToken>& tokens) {
    if(!token->Is(TokenType::IDENTIFIER))
        throw std::runtime_error("error: unexpected token while parsing identifier.");

    std::string firstValue = std::get<std::string>(token->GetValue());

    // Check if the token is a scope such as in: "scope:value"
    if(tokens.size() < 2 || !tokens.at(0)->Is(TokenType::TWO_DOTS) || !tokens.at(1)->Is(TokenType::IDENTIFIER))
        return MakeShared<Object>(firstValue);

    std::string secondValue = std::get<std::string>(tokens.at(1)->GetValue());
    tokens.pop_front();
    tokens.pop_front();
    
    return MakeShared<Object>(ScopedString(firstValue, secondValue));
}

SharedPtr<Object> Parser::Impl::ParseRange(std::deque<PToken>& tokens) {
    
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

    return MakeShared<Object>(list);
}

template<typename T>
SharedPtr<Object> Parser::Impl::ParseList(std::deque<PToken>& tokens) {
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
    
    return MakeShared<Object>(list);
}


template <>
SharedPtr<Object> Parser::Impl::ParseList<SharedPtr<Object>>(std::deque<PToken>& tokens) {
    std::vector<SharedPtr<Object>> list;
    PToken token;
    
    // The RIGHT_BRACE token must be removed from the list before returning.
    token = tokens.front();
    tokens.pop_front();

    while(!token->Is(TokenType::RIGHT_BRACE)) {
        list.push_back(Parse(tokens, 1));

        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing list.");
        
        token = tokens.front();
        tokens.pop_front();
    }
    
    return MakeShared<Object>(list);
}
    
bool Parser::Impl::IsList(std::deque<PToken>& tokens) {
    if(tokens.size() < 2)
        return false;

    PToken firstToken = tokens.at(0);
    PToken secondToken = tokens.at(1);

    #define IS_LIST_TYPE(t) (t->Is(TokenType::IDENTIFIER) || t->Is(TokenType::NUMBER) || t->Is(TokenType::BOOLEAN) || t->Is(TokenType::STRING))

    // Check if two successive tokens are of the same type.
    // So, if there isn't any operators for the second tokens,
    // it has to be a list.

    // Or if there is only an element in the list, check if the second
    // token is a RIGHT_BRACE.
    return (secondToken->Is(firstToken->GetType()) && IS_LIST_TYPE(secondToken))
        || (secondToken->Is(TokenType::RIGHT_BRACE) && IS_LIST_TYPE(firstToken))
        || firstToken->Is(TokenType::LEFT_BRACE);
}

template <typename T>
std::string Parser::Format::FormatNumbersList(const Parser::Scalar& key, const SharedPtr<Parser::Object>& object, uint depth) {
    std::vector<T> l = (*object);
    std::vector<T> loneNumbers;
    std::string indent = std::string(depth, '\t');

    // Sort the list by ascending order.
    // Note: DO NOT sort the list because the order can matter (e.g colors).
    // std::sort(l.begin(), l.end(), [=](double a, double b) { return a < b; });

    // Make a list of the lines to build the list
    // with LIST and RANGE depending on the values.
    std::vector<std::string> lines;
    int start = 0;
    int current = 1;

    while(current <= l.size()) {
        // Count the number of elements in the current range and make
        // a range only if there are at least 3 elements.
        int n = current - start;

        // Push a new line if a streak is broken or if it is the last element of the vector.
        if(current == l.size() || l[current-1]+1 != l[current]) {
            if(n > 3) {
                lines.push_back(fmt::format("RANGE {{ {} {} }}", l[start], l[current-1]));
            }
            else {
                for(int i = start; i < current; i++)
                    loneNumbers.push_back(l[i]);
            }
            start = current;
        }
        current++;
    }

    if(!loneNumbers.empty()) {
        lines.push_back(fmt::format("LIST {{ {} }}", fmt::join(
            std::views::transform(loneNumbers, [](const auto& v) {
                return fmt::format("{}", v);
            }), " ")
        ));
    }

    return fmt::format("{}", fmt::join(
        std::views::transform(lines, [indent, key](const auto& line) {
            return fmt::format("{}{} = {}", indent, key, line);
        }), "\n")
    );
}

template std::string Parser::Format::FormatNumbersList<int>(const Scalar& key, const SharedPtr<Object>& object, uint depth);
template std::string Parser::Format::FormatNumbersList<double>(const Scalar& key, const SharedPtr<Object>& object, uint depth);

std::string Parser::Format::FormatStringsList(const Scalar& key, const SharedPtr<Object>& object, uint depth) {
    const std::vector<std::string>& strings = std::get<std::vector<std::string>>(object->AsArray());
    std::string indent = std::string(depth, '\t');
    return fmt::format("{}", fmt::join(
        std::views::transform(strings, [indent, key](const auto& string) {
            return fmt::format("{}{} = {}", indent, key, string);
        }), "\n")
    );
}

template <typename T>
bool Parser::Format::IsRange(const std::vector<T>& numbers) {
    if(numbers.size() < 3)
        return false;
    for(int i = 1; i < numbers.size(); i++) {
        if(numbers[i] != numbers[i-1]+1)
            return false;
    }
    return true;
}

template bool Parser::Format::IsRange<int>(const std::vector<int>& numbers);
template bool Parser::Format::IsRange<double>(const std::vector<double>& numbers);

std::string Parser::Format::FormatObject(const SharedPtr<Object>& object, uint depth, bool isRoot) {
    if(object->Is(Parser::ObjectType::OBJECT)) {
        // An empty object is an empty-string on first depth, { } otherwise.
        if(object->GetEntries().empty())
            return (depth > 0) ? "{ }" : "";

        // Format recursively objects in the current object.
        auto v = std::views::transform(object->GetEntries(), [depth](const auto& p) {
            if(p.second.second->Is(Parser::ObjectType::ARRAY)) {
                // if(p.second.second->GetArrayType() == Parser::ObjectType::INT)
                //     return FormatNumbersList<int>(p.first, p.second.second, depth);
                // if(p.second.second->GetArrayType() == Parser::ObjectType::DECIMAL)
                //     return FormatNumbersList<double>(p.first, p.second.second, depth);
                if(p.second.second->GetArrayType() == Parser::ObjectType::STRING)
                    return FormatStringsList(p.first, p.second.second, depth);
            }
            return fmt::format(
                FMT_COMPILE("{}{} {} {}"),
                std::string(depth, '\t'), // Indentation
                p.first, // Key
                p.second.first, // Operator
                FormatObject(p.second.second, depth+1, false) // Value
            );
        });

        // On first depth, the object is formatted as ..., instead of {...} 
        if(depth == 0 || isRoot)
            return fmt::format("{}", fmt::join(v, "\n"));
        return fmt::format("{{\n{}\n{}}}", fmt::join(v, "\n"), std::string(depth-1, '\t'));
    }
    else if(object->Is(Parser::ObjectType::ARRAY)) {
        if(object->GetArrayType() == Parser::ObjectType::OBJECT) {
            const std::vector<SharedPtr<Parser::Object>>& objects = (*object);
            if(objects.empty())
                return "{ }";
            auto v = std::views::transform(objects, [depth](const auto& o) {
                return Parser::Format::FormatObjectFlat(o, depth+1);
            });
            return fmt::format("{{\n{}\n{}}}", fmt::join(v, "\n"), std::string(std::max(0U, depth-1), '\t'));
        }
        return fmt::format("{}", object->AsArray());
    }
    return fmt::format("{}", object->AsScalar());
}

std::string Parser::Format::FormatObjectFlat(const SharedPtr<Object>& object, uint depth) {
    if(object->Is(Parser::ObjectType::OBJECT)) {
        auto v = std::views::transform(object->GetEntries(), [depth](const auto& p) {
            return fmt::format(
                FMT_COMPILE("{} {} {}"),
                p.first, // Key
                p.second.first, // Operator
                FormatObjectFlat(p.second.second, depth+1) // Value
            );
        });
        return fmt::format("{}{{ {} }}", std::string(std::max(0U, depth-1), '\t'), fmt::join(v, " "));
    }
    else if(object->Is(Parser::ObjectType::ARRAY)) {
        return fmt::format("{}", object->AsArray());
    }
    return fmt::format("{}", object->AsScalar());
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

    SharedPtr<Object> result = Parser::Parse(tokens);
    sf::Time elapsedParser = clock.getElapsedTime();

    fmt::println("file path = {}", filePath);
    fmt::println("file size = {}", String::FileSizeFormat(std::filesystem::file_size(filePath)));
    fmt::println("tokens = {}", tokensCount);
    fmt::println("entries = {}", result->GetEntries().size());
    fmt::println("elapsed file   = {}", String::DurationFormat(elapsedFile));
    fmt::println("elapsed lexer  = {}", String::DurationFormat(elapsedLexer));
    fmt::println("elapsed parser = {}", String::DurationFormat(elapsedParser));
    fmt::println("elapsed total  = {}", String::DurationFormat(elapsedFile + elapsedLexer + elapsedParser));

    // Display the result.
    fmt::println("\n------------------RESULT--------------------\n");
    fmt::println("{}", result);
    fmt::println("\n--------------------------------------------\n");

    ///////////////////////////
    // Test default.map file //
    ///////////////////////////
    
    if(filePath == "test_mod/map_data/default.map") {
        fmt::println("\nlakes = {}", result->GetObject("lakes"));
        fmt::println("\nisland_region => {}", result->GetObject("island_region"));
    }

    ///////////////////////////////
    // Test parser_test.txt file //
    ///////////////////////////////

    if(filePath == "test_mod/parser_test.txt") {
        // Test number list.
        fmt::println("\nlist = {}", result->GetObject("list"));
            
        // Test range.
        fmt::println("\nrange = {}", result->GetObject("range"));
        
        // Test string list.
        fmt::println("\nstring_list = {}", result->GetObject("string_list"));
        
        // Test for single raw values.
        fmt::println("\nidentifier => {}", result->GetObject("identifier"));
        fmt::println("string => {}", result->GetObject("string"));
        fmt::println("number => {}", result->GetObject("number"));
        fmt::println("bool => {}", result->GetObject("bool"));
        fmt::println("date => {}", result->GetObject("date"));
        fmt::println("scope:value => {}", result->GetObject(ScopedString("scope", "value")));
        // fmt::println("scope => {}", (ScopedString) result.GetObject("scope"));
        
        // Test date as key
        fmt::println("1000.1.1 => {}", result->GetObject(Date(1000, 1, 1)));
        
        // Test utf8 characters as value
        fmt::println("utf8 => {}", result->GetObject("utf8"));
        
        fmt::println("operators = {}", result->GetObject("operators"));
    }
}

void Parser::Tests() {
    std::string dir = "tests/parser/";
    SharedPtr<Object> data;

    const auto& SerializeList = [](const auto& l) {
        std::string s = fmt::format(
            "[{}]",
            fmt::join(
                std::views::transform(l, [](const auto& v) { return fmt::format("{}", v); }),
                ", "
            )
        );
        s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
        s.erase(std::remove(s.begin(), s.end(), '\t'), s.end());
        return s;
    };

    #define ASSERT(name, expected, got) if(expected != got) throw std::runtime_error(fmt::format("Failed tests at line {} for {}, expected {}, got {}", __LINE__, name, expected, got))

    // Tests : Scalars (int, decimal, bool, string, date, scoped string)
    try {
        data = Parser::ParseFile(dir + "scalars.txt");
        ASSERT("int", 1234.0, data->Get<double>("i1"));
        ASSERT("negative int", -1234.0, data->Get<double>("i2"));
        ASSERT("double", 10.234, data->Get<double>("d1"));
        ASSERT("negative double", -10.234, data->Get<double>("d2"));
        ASSERT("bool true", true, data->Get<bool>("b_yes"));
        ASSERT("bool false", false, data->Get<bool>("b_no"));
        ASSERT("word", "Lorem", data->Get<std::string>("s1"));
        ASSERT("string", "\"Lorem ipsum dolor sit amet\"", data->Get<std::string>("s2"));
        ASSERT("date1", Date(14, 8, 19), data->Get<Date>("date1"));
        ASSERT("date2", Date(1453, 5, 29), data->Get<Date>("date2"));
        ASSERT("scoped string", ScopedString("culture", "roman"), data->Get<ScopedString>("culture"));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'scalars.txt'\n{}", e.what()));
    }
    
    // Tests : Arrays
    try {
        data = Parser::ParseFile(dir + "arrays.txt");
        ASSERT("int list", "[100, 50, 200, -25]", SerializeList(data->GetArray<double>("int_list", {})));
        ASSERT("double list", "[100.52, -50.99]", SerializeList(data->GetArray<double>("double_list", {})));
        ASSERT("bool list", "[true, false, false, true]", SerializeList(data->GetArray<bool>("bool_list", {})));
        ASSERT("string list", "[breton, french, \"Lorem ipsum dolor sit amet\", norse]", SerializeList(data->GetArray<std::string>("string_list", {})));
        // ASSERT("date list", "[breton, french, \"Lorem ipsum dolor sit amet\", norse]", SerializeList(data->GetArray<Date>("date_list", {})));
        // ASSERT("scoped string list", "[culture:roman]", SerializeList(data->GetArray<ScopedString>("scoped_string_list", {})));
        ASSERT("node list", "[name = augustus, name = claudius, name = nero]", SerializeList(data->GetArray<SharedPtr<Object>>("node_list", {})));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'arrays.txt'\n{}", e.what()));
    }
    
    // Tests : Keys (scalars)
    try {
        data = Parser::ParseFile(dir + "keys.txt");
        ASSERT("int key", true, data->ContainsKey(10.0));
        ASSERT("int key", "key1", data->Get<std::string>(10.0));
        
        ASSERT("decimal key", true, data->ContainsKey(3.125));
        ASSERT("decimal key", "key2", data->Get<std::string>(3.125));
        
        ASSERT("bool key", true, data->ContainsKey(true));
        ASSERT("bool key", "key3", data->Get<std::string>(true));
        
        ASSERT("string1 key", true, data->ContainsKey("string1"));
        ASSERT("string1 key", "key4", data->Get<std::string>("string1"));
        
        ASSERT("string2 key", true, data->ContainsKey("\"string2\""));
        ASSERT("string2 key", "key5", data->Get<std::string>("\"string2\""));
        
        ASSERT("date key", true, data->ContainsKey(Date(14, 8, 19)));
        ASSERT("date key", "key6", data->Get<std::string>(Date(14, 8, 19)));
        
        ASSERT("scoped string key", true, data->ContainsKey(ScopedString("culture", "roman")));
        ASSERT("scoped string key", "key7", data->Get<std::string>(ScopedString("culture", "roman")));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'keys.txt'\n{}", e.what()));
    }

    // Tests : Operators
    try {
        data = Parser::ParseFile(dir + "operators.txt");
        ASSERT("eq operator", Parser::Operator::EQUAL, data->GetOperator("op1"));
        ASSERT("lt operator", Parser::Operator::LESS, data->GetOperator("op2"));
        ASSERT("le operator", Parser::Operator::LESS_EQUAL, data->GetOperator("op3"));
        ASSERT("gt operator", Parser::Operator::GREATER, data->GetOperator("op4"));
        ASSERT("ge operator", Parser::Operator::GREATER_EQUAL, data->GetOperator("op5"));
        ASSERT("neq operator", Parser::Operator::NOT_EQUAL, data->GetOperator("op6"));
        ASSERT("exists operator", Parser::Operator::EXIST, data->GetOperator("op7"));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'operators.txt'\n{}", e.what()));
    }
    
    // Tests : Append to array
    try {
        data = Parser::ParseFile(dir + "arrays_append.txt");
        ASSERT("int append list", "[10, 20, 50]", SerializeList(data->GetArray<double>("ints", {})));
        ASSERT("double append list", "[3.125, 5, 2.7]", SerializeList(data->GetArray<double>("decimals", {})));
        ASSERT("bool append list", "[true, false, false]", SerializeList(data->GetArray<bool>("booleans", {})));
        ASSERT("string append list", "[greedy, compassionate, brave]", SerializeList(data->GetObject("character")->GetArray<std::string>("trait", {})));
        // ASSERT("date append list", "[]", SerializeList(data->GetArray<Date>("dates", {})));
        // ASSERT("scoped string append list", "[]", SerializeList(data->GetArray<ScopedString>("scoped_strings", {})));
        ASSERT("node append list", "[name = \"holy_order_name1\"coat_of_arms = \"holy_order_coa1\", name = \"holy_order_name2\"coat_of_arms = \"holy_order_coa2\"]", SerializeList(data->GetArray<SharedPtr<Object>>("holy_order_names", {})));
        // fmt::println("{}", data->GetObject("test"));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'arrays_append.txt'\n{}", e.what()));
    }
    
    // Tests : Depth
    try {
        data = Parser::ParseFile(dir + "depth.txt");
        ASSERT("initial", 1, data->GetEntries().size());
        ASSERT("depth 1", true, (data->GetObject("depth1") != nullptr));
        ASSERT("depth 2", true, (data->GetObject("depth1")->GetObject("depth2") != nullptr));
        ASSERT("depth 3a", true, (data->GetObject("depth1")->GetObject("depth2")->GetObject("depth3a") != nullptr));
        ASSERT("depth 3b", true, (data->GetObject("depth1")->GetObject("depth2")->GetObject("depth3b") != nullptr));
        ASSERT("depth 4", true, (data->GetObject("depth1")->GetObject("depth2")->GetObject("depth3a")->GetObject("depth4") != nullptr));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'depth.txt'\n{}", e.what()));
    }
    
    // Tests : Colors
    try {
        data = Parser::ParseFile(dir + "colors.txt");
        ASSERT("hsv1 color", sf::Color(77, 70, 61), data->Get<sf::Color>("c1"));
        ASSERT("hsv2 color", sf::Color(225, 230, 207), data->Get<sf::Color>("c2"));
        ASSERT("rgb1 color", sf::Color(153, 2, 34), data->Get<sf::Color>("c3"));
        ASSERT("rgb2 color", sf::Color(23, 21, 99), data->Get<sf::Color>("c4"));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'colors.txt'\n{}", e.what()));
    }

    // Tests : Ranges
    try {
        data = Parser::ParseFile(dir + "ranges.txt");
        ASSERT("range list", "[1, 2, 3, 4, 5]", SerializeList(data->GetArray<double>("l1", {})));
        ASSERT("transform to range", "RANGE { 1 5 }", fmt::format("{}", data->GetObject("l1")));
        ASSERT("concatenate list", "[1, 2, 3, 4, 5, 6, 7, 8]", SerializeList(data->GetArray<double>("l2", {})));
        ASSERT("transform to range", "RANGE { 1 8 }", fmt::format("{}", data->GetObject("l2")));
        ASSERT("range list", "l3 = RANGE { 5 11 }", fmt::format("{}", data->GetObject("l3")));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'ranges.txt'\n{}", e.what()));
    }
    
    // Tests : Formatting
    try {
        data = Parser::ParseFile(dir + "formatting.txt");

        ASSERT("int", "10", fmt::format("{}", data->GetObject("int")));
        ASSERT("decimal", "3.1415", fmt::format("{}", data->GetObject("decimal")));
        ASSERT("yes", "yes", fmt::format("{}", data->GetObject("bool1")));
        ASSERT("no", "no", fmt::format("{}", data->GetObject("bool2")));
        ASSERT("string", "word", fmt::format("{}", data->GetObject("string")));
        ASSERT("quoted string", "\"Hello World!\"", fmt::format("{}", data->GetObject("quoted_string")));
        ASSERT("date", "14.8.19", fmt::format("{}", data->GetObject("date")));
        ASSERT("scoped string", "culture:roman", fmt::format("{}", data->GetObject("scoped_string")));
        
        ASSERT("int list", "{ 10 2 5 }", fmt::format("{}", data->GetObject("int_list")));
        ASSERT("decimal list", "{ 1.1 1.3 1.2 }", fmt::format("{}", data->GetObject("decimal_list")));
        ASSERT("bool list", "{ yes yes no }", fmt::format("{}", data->GetObject("bool_list")));
        ASSERT("string list", "{ word1 word2 word3 }", fmt::format("{}", data->GetObject("string_list")));

        ASSERT("eq operator", "=", fmt::format("{}", data->GetOperator("op1")));
        ASSERT("lt operator", "<", fmt::format("{}", data->GetOperator("op2")));
        ASSERT("le operator", "<=", fmt::format("{}", data->GetOperator("op3")));
        ASSERT("gt operator", ">", fmt::format("{}", data->GetOperator("op4")));
        ASSERT("ge operator", ">=", fmt::format("{}", data->GetOperator("op5")));

        ASSERT("character", "name = Gaius\nculture = culture:roman\nadd_trait = greedy\nadd_trait = brave\n14.8.19 = {\n\tbirth = yes\n}\nnumbers = { -10 24 24213421 }\nnames = {\n\t{ name = a }\n\t{ name = b }\n}", fmt::format("{}", data->GetObject("character")));
        ASSERT("depth", "depth2 = {\n\tdepth3a = {\n\t\tdepth4 = { }\n\t}\n\tdepth3b = { }\n}", fmt::format("{}", data->GetObject("depth1")));
        ASSERT("file", "int = 10\ndecimal = 3.1415\nbool1 = yes\nbool2 = no\nstring = word\nquoted_string = \"Hello World!\"\ndate = 14.8.19\nscoped_string = culture:roman\nint_list = { 10 2 5 }\ndecimal_list = { 1.1 1.3 1.2 }\nbool_list = { yes yes no }\nstring_list = word1\nstring_list = word2\nstring_list = word3\nop1 = 0\nop2 < 0\nop3 <= 0\nop4 > 0\nop5 >= 0\ncharacter = {\n\tname = Gaius\n\tculture = culture:roman\n\tadd_trait = greedy\n\tadd_trait = brave\n\t14.8.19 = {\n\t\tbirth = yes\n\t}\n\tnumbers = { -10 24 24213421 }\n\tnames = {\n\t\t{ name = a }\n\t\t{ name = b }\n\t}\n}\ndepth1 = {\n\tdepth2 = {\n\t\tdepth3a = {\n\t\t\tdepth4 = { }\n\t\t}\n\t\tdepth3b = { }\n\t}\n}", fmt::format("{}", data));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'formatting.txt'\n{}", e.what()));
    }

    // Tests : Order
    try {
        data = Parser::ParseFile(dir + "order.txt");

        ASSERT("order", "[key1, key2, key0, key3]", SerializeList(data->GetKeys()));
        ASSERT("order 2", "[key2, key1]", SerializeList(data->GetObject("key3")->GetKeys()));
    }
    catch(std::exception& e) {
        throw std::runtime_error(fmt::format("Failed to parse 'order.txt'\n{}", e.what()));
    }
    
    // exit(0);
}