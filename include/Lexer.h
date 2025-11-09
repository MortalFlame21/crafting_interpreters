#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <any>

using namespace std::string_view_literals;

class Token {
public:
    enum class Type {
        // Single
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        BACK_SLASH,
        FORWARD_SLASH,
        ASTERISK,

        // Double
        EXCLAIM,
        EXCLAIM_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,

        EOF_TOKEN,
        MAX_TOKEN_TYPE,
        UNKNOWN_TOKEN_TYPE = MAX_TOKEN_TYPE,
    };

    Token(Type type, std::string_view lexeme, std::any literal,
        std::size_t line);

    std::string str();
    static std::string anyToString(const std::any& anyLiteral);

    Type m_type{};
    std::string m_lexeme{};
    std::any m_literal{};
    std::size_t m_line{};
};

class Lexer {
public:
    Lexer(std::string_view src);
    std::vector<Token> scanTokens();

private:
    std::string m_src {};
    std::vector<Token> m_tokens {};
    std::size_t m_start {};
    std::size_t m_current {};
    std::size_t m_line { 1 };
    static inline std::unordered_map<std::string_view, Token::Type> m_keywords {
        { "and", Token::Type::AND },
        { "class", Token::Type::CLASS },
        { "else", Token::Type::ELSE },
        { "false", Token::Type::FALSE },
        { "for", Token::Type::FOR },
        { "fun", Token::Type::FUN },
        { "if", Token::Type::IF },
        { "nil", Token::Type::NIL },
        { "or", Token::Type::OR },
        { "print", Token::Type::PRINT },
        { "return", Token::Type::RETURN },
        { "super", Token::Type::SUPER },
        { "return", Token::Type::RETURN },
        { "this", Token::Type::THIS },
        { "true", Token::Type::TRUE },
        { "var", Token::Type::VAR },
        { "while", Token::Type::WHILE },
    };

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(Token::Type tokenType);
    void addToken(Token::Type tokenType, std::any literal);
    bool match(char expectedToken);
    char peek();
    void string();
    void number();
    bool isDigit(char c);
    char peekNext();
    bool isAlpha(char c);
    void identifier();
    bool isAlnum(char c);
};