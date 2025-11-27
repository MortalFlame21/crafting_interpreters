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

Token Parser::consume(Token::Type type, const std::string& msg) {
    if (check(type)) return advance();
    throw error(peek(), msg);
}

Parser::ParseError Parser::error(Token token, const std::string& msg) {
    Errors::errors(token, msg);
    return ParseError(msg);
}

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements {};

    while (!isAtEnd()) {
        statements.push_back(std::move(declaration()));
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

std::unique_ptr<Expression> Parser::finishCall(std::unique_ptr<Expression> callee) {
    std::vector<std::unique_ptr<Expression>> args {};

    if (!check(Token::Type::RIGHT_PAREN)) {
        do {
            if (args.size() >= MAX_ARG_SIZE)
                error(peek(), "Can't have more than" + std::to_string(MAX_ARG_SIZE)
                    + " arguments");
            args.push_back(expression());
        }
        while (match({ Token::Type::COMMA }));
    }

    auto paren { consume(Token::Type::RIGHT_PAREN, "Expect ')' after arguments")};

    return std::make_unique<Call> (
        std::move(callee), std::move(paren), std::move(args)
    );
}

std::unique_ptr<Statement> Parser::declaration() {
    try {
        if (match({ Token::Type::VAR })) return varDeclaration();
        return statement();
    } catch (ParseError& e) {
        synchronise();
        return {};
    }
}

std::unique_ptr<Statement> Parser::varDeclaration() {
    auto name { consume(Token::Type::IDENTIFIER, "Expect variable name" )};

    std::unique_ptr<Expression> expr {};
    if (match({ Token::Type::EQUAL }))
        expr = expression();

    consume(Token::Type::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VariableStmt>(name, std::move(expr));
}

std::unique_ptr<Statement> Parser::statement() {
    if (match({ Token::Type::FOR })) return forStatement();
    if (match({ Token::Type::IF })) return ifStatement();
    if (match({ Token::Type::WHILE })) return whileStatement();
    if (match({ Token::Type::PRINT })) return printStatement();
    if (match({ Token::Type::LEFT_BRACE }))
        return std::make_unique<BlockStmt>(std::move(block()));
    return expressionStatement();
}

std::unique_ptr<Statement> Parser::forStatement() {
    consume(Token::Type::LEFT_PAREN, "Expect '(' after \"for\"");

    // initializer
    std::unique_ptr<Statement> init {};
    if (match({ Token::Type::SEMICOLON }))
        init = nullptr;
    else if (match({ Token::Type::VAR }))
        init = varDeclaration();
    else
        init = expressionStatement();

    // condition
    auto condition { (!check(Token::Type::SEMICOLON))
                        ? expression() : nullptr };
    consume(Token::Type::SEMICOLON, "Expect ';' after condition");

    // increment
    auto increment { (!check(Token::Type::SEMICOLON))
                        ? expression() : nullptr };
    consume(Token::Type::RIGHT_PAREN, "Expect ')' after for clause");

    // body
    auto body { statement() };
    if (increment) {
        std::vector<std::unique_ptr<Statement>> stmts {};
        stmts.push_back(std::move(body));
        stmts.push_back(std::make_unique<ExpressionStmt>(std::move(increment)));
        body = std::make_unique<BlockStmt>(std::move(stmts));
    }

    if (!condition)
        condition = std::make_unique<Literal>(true);

    body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

    if (init) {
        std::vector<std::unique_ptr<Statement>> stmts {};
        stmts.push_back(std::move(init));
        stmts.push_back(std::move(body));
        body = std::make_unique<BlockStmt>(std::move(stmts));
    }

    return body;
}

std::unique_ptr<Statement> Parser::whileStatement() {
    consume(Token::Type::LEFT_PAREN, "Expect '(' after \"while\"");
    auto condition { expression() };
    consume(Token::Type::RIGHT_PAREN, "Expect ')' after condition");
    auto body { statement() };

    return std::make_unique<WhileStmt> (
        std::move(condition),
        std::move(body)
    );
}

std::unique_ptr<Statement> Parser::ifStatement() {
    consume(Token::Type::LEFT_PAREN, "Expect '(' after \"if\"");
    auto condition { expression() };
    consume(Token::Type::RIGHT_PAREN, "Expect ')' after if condition");

    auto thenBranch { statement() };
    auto elseBranch { (match({ Token::Type::ELSE }))
                        ? statement() : nullptr };

    return std::make_unique<IfStmt> (
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch)
    );
}

std::vector<std::unique_ptr<Statement>> Parser::block() {
    std::vector<std::unique_ptr<Statement>> statements {};

    while (!check(Token::Type::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(Token::Type::RIGHT_BRACE, "Expect '}' at end of block");
    return statements;
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    auto expr { expression() };
    consume(Token::Type::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Statement> Parser::printStatement() {
    auto expr { expression() };
    consume(Token::Type::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Expression> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expression> Parser::assignment() {
    auto expr { logicalOr() };

    if (match({ Token::Type::EQUAL })) {
        auto equals { previous() };
        auto value { assignment() };

        // so wack but idk
        if (auto* varExpr { dynamic_cast<Variable*>(expr.get()) }) {
            auto name { varExpr->m_name };
            return std::make_unique<Assignment>(name, std::move(value));
        }

        error(equals, "Invalid assignment target");
    }

    return expr;
}

std::unique_ptr<Expression> Parser::logicalOr() {
    auto expr { logicalAnd() };

    while (match({ Token::Type::OR })) {
        auto operator_ { previous() };
        auto right { logicalAnd() };

        expr = std::make_unique<Logical> (
            std::move(expr),
            std::move(right),
            std::move(operator_)
        );
    }

    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd() {
    auto expr { equality() };

    while (match({ Token::Type::AND })) {
        auto operator_ { previous() };
        auto right { equality() };

        expr = std::make_unique<Logical> (
            std::move(expr),
            std::move(right),
            std::move(operator_)
        );
    }

    return expr;
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

    return call();
}

std::unique_ptr<Expression> Parser::call() {
    auto expr { primary() };

    while (match({ Token::Type::LEFT_PAREN })) {
        expr = finishCall(std::move(expr));
    }

    return expr;
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

    if (match({ Token::Type::IDENTIFIER }))
        return std::make_unique<Variable>(previous());

    throw error(peek(), "Expect expression.");
}