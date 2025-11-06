#pragma once

#include <iostream>
#include <memory>
#include <any>

#include "Lexer.h"
#include "Expression.h"

class Interpreter : public Visitor {
public:
	std::any visitBinary(const Binary& binary) override {

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
    std::any evaluate(Expression* expression) {
        return expression->accept(*this);
    }

    bool isTruthy(std::any object) {
       if (!object.has_value()) return false;
       if (object.type() == typeid(bool)) return std::any_cast<bool>(object);
       return true;
    }
};