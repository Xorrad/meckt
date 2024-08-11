#pragma once
#include "Reader.hpp"

namespace Parser {
    using PToken = SharedPtr<Token>;
    using TokenValue = std::variant<double, bool, std::string, Date, PToken>;

    enum class TokenType {
        // Separators / Punctuators
        LEFT_BRACE, RIGHT_BRACE, TWO_DOTS,

        // Operators
        EQUAL,
        LESS, LESS_EQUAL,
        GREATER, GREATER_EQUAL, 

        // Literals
        STRING, NUMBER, BOOLEAN, DATE,
        
        // Others
        COMMENT, IDENTIFIER
    };

    class Token {
    public:
        Token(TokenType type);
        Token(TokenType type, TokenValue value);
        Token(const Token& token);
        ~Token();

        TokenType GetType() const;
        bool Is(TokenType type) const;
        TokenValue GetValue() const;

    private:
        TokenType m_Type;
        TokenValue m_Value;
    };

    std::deque<PToken> Lex(const std::string& content);

    PToken ReadToken(Reader& reader);
    PToken ReadString(Reader& reader);
    PToken ReadNumber(Reader& reader);
    PToken ReadIdentifier(Reader& reader);
}