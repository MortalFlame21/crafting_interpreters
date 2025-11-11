#pragma once

#include <iostream>
#include <stdexcept>
#include <any>

#include "Lexer.h"
#include "Expression.h"

class Interpreter : public Expression::Visitor {
public:
    class RuntimeError : public std::runtime_error {
        Token m_token;

        RuntimeError(Token token, const std::string& error)
            : std::runtime_error{ error }, m_token{ token } { };

        friend class Interpreter;
        friend class Lox;
    };

	std::any visitBinary(const Binary& binary) override;
	std::any visitGrouping(const Grouping& grouping) override;
	std::any visitLiteral(const Literal& literal) override;
	std::any visitUnary(const Unary& unary) override;
    void interpret(Expression* expression);

private:
    std::any evaluate(Expression* expression);
    bool isTruthy(std::any object);
    bool isEqual(std::any left, std::any right);
    void checkNumberOperand(Token operator_, std::any operand);
    void checkNumberOperands(Token operator_, std::any left, std::any right);
    std::string str(std::any object);
};