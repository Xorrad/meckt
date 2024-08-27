#pragma once

#include "parser/Lexer.hpp"

#include <fmt/format.h>
#include <ranges>

namespace Parser {
    using Key = std::variant<double, std::string, Date, ScopedString>;
    using RawValue = std::variant<double, bool, std::string, Date, ScopedString, std::vector<double>, std::vector<bool>, std::vector<std::string>>;

    enum class Operator {
        EQUAL,
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
    };
    std::string OperatorToString(Operator op);

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
            Node(const sf::Color& color);
            Node(const std::map<Key, std::pair<Operator, Node>>& values);

            ValueType GetType() const;
            bool Is(ValueType type) const;
            bool IsList() const;

            uint GetDepth() const;
            void SetDepth(uint depth);

            // Functions to use with LeafHolder.
            void Push(const RawValue& value);

            // Functions to use with NodeHolder.
            Node& Get(const Key& key);
            const Node& Get(const Key& key) const;
            template <typename T> T Get(const Key& key, T defaultValue) const;
            Operator GetOperator(const Key& key);
            std::map<Key, std::pair<Operator, Node>>& GetEntries();
            const std::map<Key, std::pair<Operator, Node>>& GetEntries() const;
            std::vector<Key> GetKeys() const;
            bool ContainsKey(const Key& key) const;
            void Put(const Key& key, const Node& node, Operator op = Operator::EQUAL);
            void Put(const Key& key, const RawValue& value, Operator op = Operator::EQUAL);
            Node Remove(const Key& key);

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
            uint m_Depth;
    };

    class AbstractValueHolder {
        public:
            virtual ValueType GetType() const = 0;
            virtual SharedPtr<AbstractValueHolder> Copy() const = 0;
            virtual void SetDepth(uint depth) = 0;
    };

    class NodeHolder : public AbstractValueHolder {
        friend Node;

        public:
            NodeHolder();
            NodeHolder(const NodeHolder& n);
            NodeHolder(const std::map<Key, std::pair<Operator, Node>>& values);

            virtual ValueType GetType() const;
            virtual SharedPtr<AbstractValueHolder> Copy() const;
            virtual void SetDepth(uint depth);

        private:
            std::map<Key, std::pair<Operator, Node>> m_Values;
    };

    class LeafHolder : public AbstractValueHolder {
        friend Node;

        public:
            LeafHolder();
            LeafHolder(const LeafHolder& l);
            LeafHolder(const RawValue& value);

            virtual ValueType GetType() const;
            virtual SharedPtr<AbstractValueHolder> Copy() const;
            virtual void SetDepth(uint depth);

        private:
            RawValue m_Value;
    };


    Node Parse(const std::string& content);
    Node Parse(std::deque<PToken>& tokens, uint depth = 0);

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
                if(p.second.second.Is(Parser::ValueType::NUMBER_LIST))
                    return formatNumbersList(p.first, p.second.second);
                return fmt::format(
                    "{}{} {} {}",
                    std::string(p.second.second.GetDepth()-1, '\t'), // Indentation
                    p.first, // Key
                    Parser::OperatorToString(p.second.first), // Operator
                    p.second.second // Value
                );
            });
            if(node.GetDepth() == 0)
                return format_to(ctx.out(), "{}", fmt::join(v, "\n"));
            return format_to(ctx.out(), "{{\n{}\n{}}}", fmt::join(v, "\n"), std::string(node.GetDepth()-1, '\t'));
        }
        return format_to(ctx.out(), "{}", (Parser::RawValue&) node);
    }

    static std::string formatNumbersList(const Parser::Key& key, const Parser::Node& node) {
        std::vector<double> l = node;
        std::vector<double> loneNumbers;
        std::string indent = std::string(node.GetDepth()-1, '\t');

        // Sort the list by ascending order.
        std::sort(l.begin(), l.end(), [=](double a, double b) { return a < b; });

        // Make a list of the lines to build the list
        // with LIST and RANGE depending on the values.
        std::vector<std::string> lines;
        int start = 0;
        int current = 1;

        while(current <= l.size()) {
            // Count the number of elements in the current range and make
            // a range only if there are at least 3 elements.
            int n = current - start;
            bool isFollowingStreak = (current < l.size() && l[current-1]+1 == l[current]);

            // Push a new line if a streak is broken or if it is the last element of the vector.
            if(!isFollowingStreak) {
                if(n > 2) {
                    lines.push_back(fmt::format("RANGE {{ {}  {} }}", l[start], l[current-1]));
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
            lines.push_back(fmt::format("{{ {} }}", fmt::join(
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
};