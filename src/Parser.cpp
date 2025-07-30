#include <string>
#include <vector>
#include <memory>

#include "Lexer.h"
#include "Expression.h"

#include "Parser.h"

Parser::Parser(std::vector<Token> tokens)
    : m_tokens{ tokens } { }

std::unique_ptr<Expression> Parser::expression() {
    return equality();
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr { std::move(comparison()) };

    while (match({
        Token::Type::EXCLAIM_EQUAL,
        Token::Type::EQUAL_EQUAL
    })) {
        auto operator_ { previous() };
        auto right { std::move(comparison()) };

        expr = std::make_unique<Binary> (
            expr, right, operator_
        );
    };

    return expr;
}

// one of the tokens must match with at least one, OR.
// would love to implement this using variadic functions
// one day...
bool Parser::match(std::vector<Token::Type> types) {
    for (auto& t : types) {
        if (check(t)) {
            advance();
            return true;
        }
    }
    return false;
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

std::unique_ptr<Expression> Parser::comparison() {
    auto expr { std::move(term()) };

    while (match({
        Token::Type::GREATER,
        Token::Type::GREATER_EQUAL,
        Token::Type::LESS,
        Token::Type::LESS_EQUAL
    })) {
        auto operator_ { previous() };
        auto right { std::move(term()) };

        expr = std::make_unique<Binary>(expr, right, operator_);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    auto expr { std::move(factor()) };

    while(match({
        Token::Type::MINUS,
        Token::Type::PLUS,
    })) {
        auto operator_ { previous() };
        auto right { std::move(factor()) };

        expr = std::make_unique<Binary>(expr, right, operator_);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    auto expr { std::move(unary()) };

    while(match({
        Token::Type::BACK_SLASH,
        Token::Type::ASTERISK,
    })) {
        auto operator_ { previous() };
        auto right { std::move(unary()) };

        expr = std::make_unique<Binary> (expr, right, operator_);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({
        Token::Type::EXCLAIM,
        Token::Type::MINUS
    })) {
        auto operator_ { previous() };
        auto right { std::move(unary()) };

        return std::make_unique<Unary>(right, operator_);
    }

    return primary();
}

std::unique_ptr<Expression> Parser::primary() {
    if (match({ Token::Type::FALSE })) return std::make_unique<Literal>(false);
    if (match({ Token::Type::TRUE })) return std::make_unique<Literal>(true);
    if (match({ Token::Type::NIL })) return std::make_unique<Literal>(nullptr);

    if (match({ Token::Type::NUMBER, Token::Type::STRING }))
        return std::make_unique<Literal>(previous().m_literal);

    if (match({ Token::Type::LEFT_PAREN })) {
        auto expr { std::move(expression()) };
        consume(Token::Type::RIGHT_PAREN, "Expect ')' after <expression>.");
        return std::make_unique<Grouping>(expr);
    }
}
