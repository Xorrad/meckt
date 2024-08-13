#pragma once

#include "parser/Lexer.hpp"

#include <fmt/format.h>
#include <ranges>

namespace Parser {
    using Key = std::variant<double, std::string, Date, ScopedString>;
    using RawValue = std::variant<double, bool, std::string, Date, ScopedString, std::vector<double>, std::vector<bool>, std::vector<std::string>>;

    enum class ValueType {
        NUMBER,
        BOOL,
        STRING,
        DATE,
        SCOPED_STRING,
        NUMBER_LIST,
        BOOL_LIST,
        STRING_LIST,
        NODE
    };

    class Node {
        public:
            Node();
            Node(const Node& node);
            Node(const RawValue& value);
            Node(const std::map<Key, Node>& values);

            ValueType GetType() const;
            bool Is(ValueType type) const;
            bool IsList() const;

            // Functions to use with LeafHolder.
            void Push(const RawValue& value);

            // Functions to use with NodeHolder.
            Node& Get(const Key& key);
            const Node& Get(const Key& key) const;
            std::map<Key, Node>& GetEntries();
            const std::map<Key, Node>& GetEntries() const;
            // std::vector<Node&> GetValues();
            std::vector<Key> GetKeys() const;
            bool ContainsKey(const Key& key) const;
            void Put(const Key& key, const Node& node);

            // Overload cast for LeafHolder.
            operator int() const;
            operator double() const;
            operator bool() const;
            operator std::string() const;
            operator Date() const;
            operator ScopedString() const;
            operator std::vector<double>&() const;
            operator std::vector<bool>&() const;
            operator std::vector<std::string>&() const;
            operator RawValue&() const;
            operator Key() const;
            operator sf::Color() const;

            Node& operator=(const RawValue& value);
            Node& operator=(const Node& value);

            Node& operator [](const Key& key);
            const Node& operator [](const Key& key) const;
        
        private:
            // Function to access the underlying value holder.
            SharedPtr<NodeHolder> GetNodeHolder();
            const SharedPtr<NodeHolder> GetNodeHolder() const;

            SharedPtr<LeafHolder> GetLeafHolder();
            const SharedPtr<LeafHolder> GetLeafHolder() const;

        private:
            SharedPtr<AbstractValueHolder> m_Value;
    };

    class AbstractValueHolder {
        public:
            virtual ValueType GetType() const = 0;
            virtual SharedPtr<AbstractValueHolder> Copy() const = 0;
    };

    class NodeHolder : public AbstractValueHolder {
        friend Node;

        public:
            NodeHolder();
            NodeHolder(const NodeHolder& n);
            NodeHolder(const std::map<Key, Node>& values);

            virtual ValueType GetType() const;
            virtual SharedPtr<AbstractValueHolder> Copy() const;

        private:
            std::map<Key, Node> m_Values;
    };

    class LeafHolder : public AbstractValueHolder {
        friend Node;

        public:
            LeafHolder();
            LeafHolder(const LeafHolder& l);
            LeafHolder(const RawValue& value);

            virtual ValueType GetType() const;
            virtual SharedPtr<AbstractValueHolder> Copy() const;

        private:
            RawValue m_Value;
    };


    Node Parse(const std::string& content);
    Node Parse(std::deque<PToken>& tokens);
    
    namespace Impl {
        Node ParseNode(std::deque<PToken>& tokens);
        Node ParseRaw(PToken token, std::deque<PToken>& tokens);
        Node ParseIdentifier(PToken token, std::deque<PToken>& tokens);
        Node ParseRange(std::deque<PToken>& tokens);

        template<typename T>
        Node ParseList(std::deque<PToken>& tokens);

        bool IsList(std::deque<PToken>& tokens);
    }

    void Benchmark();
}

///////////////////////////////////////////
//          Formatters for fmt           //
///////////////////////////////////////////
template <>
class fmt::formatter<Parser::Key> {
public:
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Parser::Key& key, Context& ctx) const {
        switch(key.index()) {
            case 0: return format_to(ctx.out(), "{}", std::get<double>(key));
            case 1: return format_to(ctx.out(), "{}", std::get<std::string>(key));
            case 2: return format_to(ctx.out(), "{}", std::get<Date>(key));
            case 3: return format_to(ctx.out(), "{}", std::get<ScopedString>(key));
        }
        return format_to(ctx.out(), "");
    }
};

template <>
class fmt::formatter<Parser::RawValue> {
public:
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Parser::RawValue& value, Context& ctx) const {
        switch((Parser::ValueType) value.index()) {
            case Parser::ValueType::NUMBER:
                return format_to(ctx.out(), "{}", std::get<double>(value));
            case Parser::ValueType::BOOL:
                return format_to(ctx.out(), "{}", std::get<bool>(value));
            case Parser::ValueType::STRING:
                return format_to(ctx.out(), "{}", std::get<std::string>(value));
            case Parser::ValueType::DATE:
                return format_to(ctx.out(), "{}", std::get<Date>(value));
            case Parser::ValueType::SCOPED_STRING:
                return format_to(ctx.out(), "{}", std::get<ScopedString>(value));
            case Parser::ValueType::NUMBER_LIST:
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(std::get<std::vector<double>>(value), [](const auto& v) {
                        return fmt::format("{}", v);
                    }), " ")
                );
            case Parser::ValueType::BOOL_LIST:
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(std::get<std::vector<bool>>(value), [](const auto& v) {
                        return fmt::format("{}", v);
                    }), " ")
                );
            case Parser::ValueType::STRING_LIST:
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(std::get<std::vector<std::string>>(value), [](const auto& v) {
                        return fmt::format("{}", v);
                    }), " ")
                );
            default:
                return format_to(ctx.out(), "");
        }
    }
};

template <>
class fmt::formatter<Parser::Node> {
public:
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Parser::Node& node, Context& ctx) const {
        if(node.Is(Parser::ValueType::NODE)) {
            auto v = std::views::transform(node.GetEntries(), [](const auto& p) {
                return fmt::format("{} = {}", p.first, p.second);
            });
            return format_to(ctx.out(), "{{\n{}\n}}", fmt::join(v, "\n"));
        }
        return format_to(ctx.out(), "{}", (Parser::RawValue&) node);
    }
};