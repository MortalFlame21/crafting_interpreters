#include <iostream>
#include <vector>
#include <string_view>
#include <sstream>

#include "Lexer.h"
#include "Errors.h"

Token::Token(Type type, std::string_view lexeme, std::string_view literal,
        std::size_t line)
    : m_type { type }
    , m_lexeme { lexeme }
    , m_literal { literal }
    , m_line { line }
{
}

std::ostream& operator<<(std::ostream& out, Token::Type t) {
    std::string_view t_str{};
    switch (t) {
    case Token::Type::LEFT_PAREN:
        t_str = "(";
        break;
    case Token::Type::RIGHT_PAREN:
        t_str = ")";
        break;
    case Token::Type::LEFT_BRACE:
        t_str = "{";
        break;
    case Token::Type::RIGHT_BRACE:
        t_str = "}";
        break;
    case Token::Type::COMMA:
        t_str = ",";
        break;
    case Token::Type::DOT:
        t_str = ".";
        break;
    case Token::Type::MINUS:
        t_str = "-";
        break;
    case Token::Type::PLUS:
        t_str = "+";
        break;
    case Token::Type::SEMICOLON:
        t_str = ";";
        break;
    case Token::Type::BACK_SLASH:
        t_str = "/";
        break;
    case Token::Type::FORWARD_SLASH:
        t_str = "\\";
        break;
    case Token::Type::ASTERISK:
        t_str = "*";
        break;
    case Token::Type::EXCLAIM:
        t_str = "!";
        break;
    case Token::Type::EXCLAIM_EQUAL:
        t_str = "!=";
        break;
    case Token::Type::EQUAL:
        t_str = "=";
        break;
    case Token::Type::EQUAL_EQUAL:
        t_str = "==";
        break;
    case Token::Type::GREATER:
        t_str = ">";
        break;
    case Token::Type::GREATER_EQUAL:
        t_str = ">=";
        break;
    case Token::Type::LESS:
        t_str = "<";
        break;
    case Token::Type::LESS_EQUAL:
        t_str = "<=";
        break;
    case Token::Type::IDENTIFIER:
        t_str = "identifier";
        break;
    case Token::Type::STRING:
        t_str = "string";
        break;
    case Token::Type::NUMBER:
        t_str = "number";
        break;
    case Token::Type::AND:
        t_str = "&&";
        break;
    case Token::Type::CLASS:
        t_str = "class";
        break;
    case Token::Type::ELSE:
        t_str = "else";
        break;
    case Token::Type::FALSE:
        t_str = "false";
        break;
    case Token::Type::FUN:
        t_str = "fun";
        break;
    case Token::Type::FOR:
        t_str = "for";
        break;
    case Token::Type::IF:
        t_str = "if";
        break;
    case Token::Type::NIL:
        t_str = "nil";
        break;
    case Token::Type::OR:
        t_str = "or";
        break;
    case Token::Type::PRINT:
        t_str = "print";
        break;
    case Token::Type::RETURN:
        t_str = "return";
        break;
    case Token::Type::SUPER:
        t_str = "super";
        break;
    case Token::Type::THIS:
        t_str = "this";
        break;
    case Token::Type::TRUE:
        t_str = "true";
        break;
    case Token::Type::VAR:
        t_str = "var";
        break;
    case Token::Type::WHILE:
        t_str = "while";
        break;
    case Token::Type::EOF_TOKEN:
        t_str = "EOF";
        break;
    case Token::Type::UNKNOWN_TOKEN_TYPE:
    // fallthrough
    default:
        t_str = "UNKNOWN_TOKEN_TYPE";
        break;
    }
    return out << t_str;
}

std::string Token::str() {
    std::stringstream ss{};
    ss << "<[type]: " << m_type << ", [lexeme]: " << m_lexeme
        << ", [literal]: " << m_literal << ">";
    return ss.str();
}

Lexer::Lexer(std::string_view src)
    : m_src { src }
{
}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        m_start = m_current;
        scanToken();
    }

    m_tokens.push_back({ Token::Type::EOF_TOKEN, "", "", m_line });

    for (auto& t : m_tokens) {
        std::cout << t.str() << "\n";
    }

    return m_tokens;
};

bool Lexer::isAtEnd() {
    return m_current >= m_src.length();
}

void Lexer::scanToken() {
    char c { advance() };
    switch (c) {
    case '(':
        addToken(Token::Type::LEFT_PAREN); break;
    case ')':
        addToken(Token::Type::RIGHT_PAREN); break;
    case '{':
        addToken(Token::Type::LEFT_BRACE); break;
    case '}':
        addToken(Token::Type::RIGHT_BRACE); break;
    case ',':
        addToken(Token::Type::COMMA); break;
    case '.':
        addToken(Token::Type::DOT); break;
    case '-':
        addToken(Token::Type::MINUS); break;
    case '+':
        addToken(Token::Type::PLUS); break;
    case ';':
        addToken(Token::Type::SEMICOLON); break;
    case '*':
        addToken(Token::Type::ASTERISK); break;
    case '!':
        addToken(match('=') ? Token::Type::EXCLAIM_EQUAL : Token::Type::EXCLAIM);
        break;
    case '=':
        addToken(match('=') ? Token::Type::EQUAL_EQUAL : Token::Type::EQUAL);
        break;
    case '>':
        addToken(match('=') ? Token::Type::GREATER_EQUAL : Token::Type::GREATER);
        break;
    case '<':
        addToken(match('=') ? Token::Type::LESS_EQUAL : Token::Type::LESS);
        break;
    case '/':
        if (match('/')) {
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
        }
        else if (match('*')) {
            while (peek() != '*' && peekNext() != '/' && !isAtEnd()) {
                if (peek() == '\n') ++m_line;
                advance();
            }
            if (isAtEnd()) {
                Errors::errors(m_line, "Multi-line comment not closed.");
                return;
            }
            // consume both '*' and '/'
            advance();
            advance();
        }
        else {
            addToken(Token::Type::BACK_SLASH);
        }
        break;
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        ++m_line; break;
    case '"':
        string(); break;
    default:
        if (std::isdigit(c)) {
            number();
        }
        else if (std::isalpha(c)) {
            identifier();
        }
        else {
            Errors::errors(m_line, "Unexpected character");
        }
        break;
    }
}

char Lexer::advance() {
    return m_src.at(m_current++);
}

void Lexer::addToken(Token::Type type) {
    addTokens(type, "");
}

void Lexer::addTokens(Token::Type type, std::string_view literal) {
    std::string lexeme { m_src.substr(m_start, m_current - m_start) };
    m_tokens.push_back({ type, lexeme, literal, m_line });
}

bool Lexer::match(char expectedToken) {
    if (isAtEnd()) return false;
    if (m_src.at(m_current) != expectedToken) return false;
    ++m_current;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\n';
    return m_src.at(m_current);
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') ++m_line;
        advance();
    }

    if (isAtEnd()) {
        Errors::errors(m_line, "Unterminated string.");
        return;
    }

    // consume ending quotation, '"'.
    advance();

    std::string strType { m_src.substr(m_start + 1, m_current - m_start - 2) };
    addTokens(Token::Type::STRING, strType);
}

void Lexer::number() {
    // the whole number.
    while (std::isdigit(peek()))
        advance();

    // consume '.', scan the fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
      advance();
      while (std::isdigit(peek())) advance();
    }

    addTokens(Token::Type::NUMBER, m_src.substr(m_start, m_current - m_start));
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

char Lexer::peekNext() {
    if (m_current + 1 >= m_src.length()) return '\0';
    return m_src.at(m_current + 1);
}

bool Lexer::isAlpha(char c) {
    return (std::tolower(c) >= 'a' && std::tolower(c) <= 'z') || c == '_';
}

void Lexer::identifier() {
    while (isAlnum(peek()))
        advance();

    std::string_view typeId { m_src.substr(m_start, m_current - m_start) };
    auto type { (m_keywords.find(typeId) == m_keywords.end()) ?
        Token::Type::UNKNOWN_TOKEN_TYPE : Token::Type::IDENTIFIER };
    addToken(type);
}

bool Lexer::isAlnum(char c) {
    return isAlpha(c) || isDigit(c);
}