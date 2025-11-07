#pragma once

#include <iostream>
#include <memory>
#include <any>

#include "Lexer.h"
#include "Expression.h"

class Interpreter : public Visitor {
public:
	std::any visitBinary(const Binary& binary) override;
	std::any visitGrouping(const Grouping& grouping) override;
	std::any visitLiteral(const Literal& literal) override;
	std::any visitUnary(const Unary& unary) override;

private:
    class InterpreterRuntimeError : public std::runtime_error {
        Token::Type m_token;

        InterpreterRuntimeError(Token::Type token, const std::string& error)
            : std::runtime_error{ error }, m_token{ token } { };

        friend class Interpreter;
    };

    std::any evaluate(Expression* expression);
    bool isTruthy(std::any object);
    bool isEqual(std::any left, std::any right);
    void checkNumberOperand(Token::Type operator_, std::any operand);
};