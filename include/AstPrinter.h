#pragma once

#include <string>
#include <string_view>
#include <sstream>

#include "Expression.h"

using namespace std::string_view_literals;

class AstPrinter : public Expression::Visitor {
public:
    std::string print(Expression* expr) {
        return std::any_cast<std::string>(expr->accept(*this));
    }

	std::any visitBinary(Binary& binary) override {
        return parenthesize (
            binary.m_operator.m_lexeme,
            { binary.m_left.get(), binary.m_right.get() }
        );
    };

	std::any visitGrouping(Grouping& group) override {
        return parenthesize (
            "Grouping",
            { group.m_expression.get() }
        );
    };

	std::any visitLiteral(Literal& literal) override {
        return Token::anyToString(literal.m_value);
    };

	std::any visitUnary(Unary& unary) override {
        return parenthesize (
            unary.m_operator.m_lexeme,
            { unary.m_right.get() }
        );
    };
private:
    // would like to know how to use smart pointers instead
    // tried:
    //      - unique_ptr <- needs to reference
    //      - smart_ptr <- some compiler error, I am not
    //        smart enough yet to fix.
    std::string parenthesize (
        std::string_view name,
        [[maybe_unused]] std::vector<Expression*> expressions
    ) {
        std::stringstream ss{};
        ss << "(" << name;
        for (auto& expr : expressions) {
            ss << " " << std::any_cast<std::string>(expr->accept(*this));
        }
        ss << ")";
        return ss.str();
    }
};