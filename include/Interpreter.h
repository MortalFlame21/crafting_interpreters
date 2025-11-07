#pragma once

#include <iostream>
#include <memory>
#include <any>

#include "Lexer.h"
#include "Expression.h"

class Interpreter : public Visitor {
public:
	std::any visitBinary(const Binary& binary) override {
        std::any left { evaluate(binary.m_left.get()) };
        std::any right { evaluate(binary.m_right.get()) };

        try {
            // might need to fix probably does not cast properly
            switch (binary.m_operator.m_type) {
            // comparision
            case Token::Type::GREATER:
                return std::any_cast<double>(left) > std::any_cast<double>(right);
            case Token::Type::GREATER_EQUAL:
                return std::any_cast<double>(left) >= std::any_cast<double>(right);
            case Token::Type::LESS:
                return std::any_cast<double>(left) < std::any_cast<double>(right);
            case Token::Type::LESS_EQUAL:
                return std::any_cast<double>(left) <= std::any_cast<double>(right);
            case Token::Type::EQUAL_EQUAL:
            case Token::Type::EXCLAIM_EQUAL:
            // arithmetic
            case Token::Type::BACK_SLASH:
                return std::any_cast<double>(left) / std::any_cast<double>(right);
            case Token::Type::ASTERISK:
                return std::any_cast<double>(left) * std::any_cast<double>(right);
            case Token::Type::MINUS:
                return std::any_cast<double>(left) - std::any_cast<double>(right);
            case Token::Type::PLUS:
                // both support adding and string concatenation
                if (left.type() == typeid(double) && right.type() == typeid(double))
                    return std::any_cast<double>(left) + std::any_cast<double>(right);

                if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
                    return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);

                break;
            }
        }
        catch (const std::bad_any_cast& e)
        {
            std::cerr << "Error: std::any_cast: " << e.what() << "\n";
            return nullptr;
        }

        return nullptr;
    }

	std::any visitGrouping(const Grouping& grouping) override {
        // would like to use some shared pointer here.
        return evaluate(grouping.m_expression.get());
    }

	std::any visitLiteral(const Literal& literal) override {
        return literal.m_value;
    }

	std::any visitUnary(const Unary& unary) override {
        // would like to use some shared pointer here.
        std::any right { evaluate(unary.m_right.get()) };

        try {
            // might need to fix probably does not cast properly
            switch (unary.m_operator.m_type) {
            case Token::Type::EXCLAIM:
                return !isTruthy(right);
            case Token::Type::MINUS:
                return std::any_cast<double>(right);
            };
        }
        catch (const std::bad_any_cast& e)
        {
            std::cerr << "Error: std::any_cast: " << e.what() << "\n";
            return nullptr;
        }

        return nullptr;
    }
private:
    class InterpreterRuntimeError : public std::runtime_error {
        Token::Type m_token;

        InterpreterRuntimeError(Token::Type token, const std::string& error)
            : std::runtime_error{ error }, m_token{ token } { };

        friend class Interpreter;
    };

    std::any evaluate(Expression* expression) {
        return expression->accept(*this);
    }

    bool isTruthy(std::any object) {
       if (!object.has_value()) return false;
       if (object.type() == typeid(bool)) return std::any_cast<bool>(object);
       return true;
    }

    bool isEqual(std::any left, std::any right) {
        if (!(left.has_value() && right.has_value())) return true;
        if (!left.has_value()) return false;

        if (left.type() == typeid(double) && right.type() == typeid(double))
            return std::any_cast<double>(left) == std::any_cast<double>(right);

        if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
            return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);

        std::cerr << "Error: What are we even comparing?\n";
        return false;
    }

    void checkNumberOperand(Token::Type operator_, std::any operand) {
        if (operand.type() == typeid(double))
            return;
        throw new InterpreterRuntimeError(operator_, "Operand must be a number");
    }
};