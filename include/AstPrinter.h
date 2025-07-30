#pragma once

#include <string>
#include <string_view>
#include <sstream>

#include "Expression.h"

using namespace std::string_view_literals;

class AstPrinter : public Visitor {
public:
    std::string print(Expression* expr) {
        return std::any_cast<std::string>(expr->accept(*this));
    }

	std::any visitBinary(const Binary& binary) override {
        return parenthesize (
            binary.m_operator.m_lexeme,
            { binary.m_left.get(), binary.m_right.get() }
        );
    };

	std::any visitGrouping(const Grouping& group) override {
        return parenthesize (
            "Grouping",
            { group.m_expression.get() }
        );
    };

    // will return std::string for now...
	std::any visitLiteral(const Literal& literal) override {
        if (!literal.m_value.has_value()) return std::string("null");
        // fix this std::any conversion hard asf
        return std::string("std::any");
    };

	std::any visitUnary(const Unary& unary) override {
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