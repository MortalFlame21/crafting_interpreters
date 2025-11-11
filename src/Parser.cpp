#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Lexer.h"
#include "Expression.h"
#include "Errors.h"

#include "Lox.h"
#include "Parser.h"

Parser::Parser(std::vector<Token> tokens)
    : m_tokens{ tokens } { }

std::unique_ptr<Expression> Parser::expression() {
    return equality();
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr { comparison() };

    while (match({
        Token::Type::EXCLAIM_EQUAL,
        Token::Type::EQUAL_EQUAL
    })) {
        auto operator_ { previous() };
        auto right { comparison() };

        expr = std::make_unique<Binary> (
            std::move(expr), std::move(right), operator_
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
    auto expr { term() };

    while (match({
        Token::Type::GREATER,
        Token::Type::GREATER_EQUAL,
        Token::Type::LESS,
        Token::Type::LESS_EQUAL
    })) {
        auto operator_ { previous() };
        auto right { term() };

        expr = std::make_unique<Binary>(
            std::move(expr), std::move(right), operator_
        );
    }

    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    auto expr { factor() };

    while(match({
        Token::Type::MINUS,
        Token::Type::PLUS,
    })) {
        auto operator_ { previous() };
        auto right { factor() };

        expr = std::make_unique<Binary>(std::move(expr), std::move(right), operator_);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    auto expr { unary() };

    while(match({
        Token::Type::BACK_SLASH,
        Token::Type::ASTERISK,
    })) {
        auto operator_ { previous() };
        auto right { unary() };

        expr = std::make_unique<Binary> (std::move(expr), std::move(right), operator_);
    }

    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({
        Token::Type::EXCLAIM,
        Token::Type::MINUS
    })) {
        auto operator_ { previous() };
        auto right { unary() };

        return std::make_unique<Unary>(std::move(right), operator_);
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
        auto expr { expression() };
        consume(Token::Type::RIGHT_PAREN, "Expect ')' after <expression>.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

Token Parser::consume(Token::Type type, const std::string& msg) {
    if (check(type)) return advance();
    throw error(peek(), msg);
}

Parser::ParseError Parser::error(Token token, const std::string& msg) {
    Errors::errors(token, msg);
    return ParseError(msg);
}

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;

    while (!isAtEnd()) {
        statements.push_back(std::move(statement()));
    }

    return statements;
}

void Parser::synchronise() {
    advance();

    while (!isAtEnd()) {
        if (previous().m_type == Token::Type::SEMICOLON)
            return;

        switch (peek().m_type) {
            case Token::Type::CLASS:
            case Token::Type::FUN:
            case Token::Type::VAR:
            case Token::Type::FOR:
            case Token::Type::IF:
            case Token::Type::WHILE:
            case Token::Type::PRINT:
            case Token::Type::RETURN:
                return;
            default:
                return;
        }

        advance();
    }
}

std::unique_ptr<Statement> Parser::statement() {
    if (match({ Token::Type::PRINT })) return printStatement();
    return expressionStatement();
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    auto expr { expression() };
    consume(Token::Type::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<ExpressionStmt>(expr);
}

std::unique_ptr<Statement> Parser::printStatement() {
    auto expr { expression() };
    consume(Token::Type::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<PrintStmt>(expr);
}