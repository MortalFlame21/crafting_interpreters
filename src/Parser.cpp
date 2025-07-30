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

// one of the tokes must match with at least one, OR.
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
    auto& expr { term() };

    while (match({
        Token::Type::GREATER,
        Token::Type::GREATER_EQUAL,
        Token::Type::LESS,
        Token::Type::LESS_EQUAL
    })) {
        auto operator_ { previous() };
        auto right { term() };

        expr = Binary (
            std::make_unique<Expression>(expr),
            std::make_unique<Expression>(right),
            operator_
        );
    }

    return expr;
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

Expression& Parser::term() {
    auto& expr { factor() };

    while(match({
        Token::Type::MINUS,
        Token::Type::PLUS,
    })) {
        auto operator_ { previous() };
        auto right { factor() };

        expr = Binary (
            std::make_unique<Expression>(expr),
            std::make_unique<Expression>(right),
            operator_
        );
    }

    return expr;
}

Expression& Parser::factor() {
    auto& expr { factor() };

    while(match({
        Token::Type::MINUS,
        Token::Type::PLUS,
    })) {
        auto operator_ { previous() };
        auto right { factor() };

        expr = Binary (
            std::make_unique<Expression>(expr),
            std::make_unique<Expression>(right),
            operator_
        );
    }

    return expr;
}

Expression& Parser::term() {
    auto& expr { unary() };

    while(match({
        Token::Type::BACK_SLASH,
        Token::Type::ASTERISK,
    })) {
        auto operator_ { previous() };
        auto right { unary() };

        expr = Binary (
            std::make_unique<Expression>(expr),
            std::make_unique<Expression>(right),
            operator_
        );
    }

    return expr;
}

Expression& Parser::unary() {
    if (match({
        Token::Type::EXCLAIM,
        Token::Type::MINUS
    })) {
        auto operator_ { previous() };
        auto right { unary() };
        return Unary (
            std::make_unique<Expression>(right),
            operator_
        );
    }

    return primary();
}

Expression& Parser::primary() {
    if (match({ Token::Type::FALSE })) return Literal(false);
    if (match({ Token::Type::TRUE })) return Literal(true);
    if (match({ Token::Type::NIL })) return Literal(nullptr);

    if (match({ Token::Type::STRING, Token::Type::NUMBER }))
        return Literal(previous().m_literal);

    if (match({ Token::Type::LEFT_PAREN })) {
        auto& expr { expression() };
        consume(Token::Type::RIGHT_PAREN, "Expect ')' after <expression>.");
        return Grouping(expr)
    }

}
