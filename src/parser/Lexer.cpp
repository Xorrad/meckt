#include "Lexer.hpp"

using Parser::PToken;
using Parser::TokenValue;
using Parser::TokenType;
using Parser::Token;

Token::Token(TokenType type)
: Token(type, (PToken) nullptr) {}

Token::Token(TokenType type, TokenValue value)
: m_Type(type), m_Value(value) {}

Token::Token(const Token& token)
: Token(token.GetType(), token.GetValue()) {}

Token::~Token() {}

TokenType Token::GetType() const {
    return m_Type;
}

bool Token::Is(TokenType type) const {
    return m_Type == type;
}

TokenValue Token::GetValue() const {
    return m_Value;
}

// TODO: add a function lex from a stringstream or ifstream.

std::deque<PToken> Parser::Lex(const std::string& content) {
    std::deque<PToken> tokens;
    Reader reader(content);

    while(!reader.IsEmpty()) {
        // Save the cursor position.
        reader.Start();
        PToken token = ReadToken(reader);
        if(token != nullptr)
            tokens.push_back(token);
    }

    return tokens;
}

PToken Parser::ReadToken(Reader& reader) {
    char ch = reader.Advance();
    // int line = reader.GetLine();

    // TODO: handle UTF8 characters
    // and ZERO WIDTH NO-BREAK SPACE

    switch(ch) {
        // Ignore whitespaces.
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            break;
        case '{': return MakeShared<Token>(TokenType::LEFT_BRACE);
        case '}': return MakeShared<Token>(TokenType::RIGHT_BRACE);
        case ':': return MakeShared<Token>(TokenType::TWO_DOTS);
        case '=': return MakeShared<Token>(TokenType::EQUAL);
        case '<': return MakeShared<Token>(reader.Match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>': return MakeShared<Token>(reader.Match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '#':
            // Ignore '#' in value of token.
            reader.Start();
            reader.SkipTo('\n');
            return MakeShared<Token>(TokenType::COMMENT, reader.End());
        case '"': return ReadString(reader);
        default:
            if(String::IsDigit(ch) || ch == '-') {
                PToken token = ReadNumber(reader);
                if(token != nullptr)
                    return token;
            }
            if(String::IsAlphaNumeric(ch) || ch == '-') {
                PToken token = ReadIdentifier(reader);
                if(token != nullptr)
                    return token;
            }
            // throw std::runtime_error(fmt::format("Unexpected character '{}' ({}) at line {}.", ch, (int) ch, line));
    }
    return nullptr;
}

PToken Parser::ReadString(Reader& reader) {
    // Save cursor to discard starting quote.
    int line = reader.GetLine();

    reader.Start();
    reader.SkipTo('"');

    if(reader.IsEmpty())
        throw std::runtime_error(fmt::format("Expected end-of-string quote missing at line at line {}.", line));

    // Discard ending quote in token value.
    std::string str = reader.End();
    reader.Advance();

    return MakeShared<Token>(TokenType::STRING, "\"" + str + "\"");
}

PToken Parser::ReadNumber(Reader& reader) {
    // Check if it is a NUMBER:
    // - only digits
    // - allow one decimal '.'

    while(String::IsDigit(reader.Peek()))
        reader.Advance();
    if(reader.Peek() == '.') {
        reader.Advance();

        // Don't allow trailing floating points.
        if(!String::IsDigit(reader.Peek()))
            return nullptr;

        while(String::IsDigit(reader.Peek()))
            reader.Advance();

        // Skip if number ends by a dot because it may be a date.
        if(reader.Peek() == '.')
            return nullptr;
    }
    std::string str = reader.End();
    return MakeShared<Token>(TokenType::NUMBER, std::stod(str));
}

PToken Parser::ReadIdentifier(Reader& reader) {
    // An IDENTIFIER can have only have digits, letters, '.' and '_',
    // whereas a BOOLEAN is either 'yes' or 'no',
    // and a DATE is formatted as: yyyy.mm.dd

    while(String::IsAlphaNumeric(reader.Peek()) || reader.Peek() == '.')
        reader.Advance();
    std::string str = reader.End();

    // Check if the string is a valid boolean.
    if(str == "yes" || str == "no")
        return MakeShared<Token>(TokenType::BOOLEAN, str == "yes");

    
    // Check if the string is a valid date.
    int dots = 0;
    int firstIndex = str[0] == '-' ? 1 : 0;
    for(int i = firstIndex; i < str.size(); i++) {
        // A date cannot have the following:
        // ".0000" or "0000."
        // "*x*" with x a non-digit character
        bool digit = String::IsDigit(str[i]);
        if(((i == firstIndex || i == str.size()-1) && !digit)
            || (!digit && str[i] != '.')
            || (!digit && str[i] == '.' && str[i-1] == '.')) {
            dots = 0;
            break;
        }
        if(str[i] == '.')
            dots++;
    }
    if(dots != 2)
        return MakeShared<Token>(TokenType::IDENTIFIER, str);

    return MakeShared<Token>(TokenType::DATE, Date(str));
}