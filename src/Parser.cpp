#include <string>
#include <vector>
#include <memory>

#include "Lexer.h"
#include "Expression.h"

#include "Parser.h"

Parser::Parser(std::vector<Token> tokens)
    : m_tokens{ tokens } { }

Expression& Parser::expression() {
    return equality();
}

Expression& Parser::equality() {
    auto& expr { comparison() };

    while (match({
        Token::Type::EXCLAIM_EQUAL,
        Token::Type::EQUAL_EQUAL
    })) {
        auto operator_ { previous() };
        auto right { comparison() };
        expr = Binary (
            std::make_unique<Expression>(expr),
            std::make_unique<Expression>(right),
            operator_
        );
    };

    return expr;
}

bool Parser::match(std::vector<Token::Type> types) {
    for (auto& t : types) {
        if (check(t)) {
            advance();
            return true;
        }
    }
    return false;
}

Expression& Parser::comparison() {
    return Expression();
}

Token Parser::previous() {
    return Token(Token::Type::AND, "", "", 1);
}

bool Parser::check(Token::Type t) {
    if (isAtEnd()) return false;
    return peek().m_type == t;
}

Token Parser::advance() {
    if (!isAtEnd()) ++m_current;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().m_type == Token::Type::EOF_TOKEN;
}

Token Parser::peek() {
    return m_tokens.at(m_current);
}

Token Parser::previous() {
    return m_tokens.at(m_current - 1);
}