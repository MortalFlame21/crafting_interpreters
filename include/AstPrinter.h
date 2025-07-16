#pragma once

#include <string>
#include <sstream>

#include "Expression.h"

class AstPrinter : public Visitor<std::string> {
public:
    std::string print() {
        return "AstPrinter::print";
    }

	std::string visitBinary() override {
        return "AstPrinter::visitBinary";
    };

	std::string visitGrouping() override {
        return "AstPrinter::visitBinary";
    };

	std::string visitUnary() override {
        return "AstPrinter::visitUnary";
    };
private:
    std::string parenthesize(
        std::string_view name,
        std::vector<Expression<std::string>> expressions
    ) {
        std::stringstream ss{};
        ss << "(" << name;
        for (auto& expr : expressions)
            ss << " " << expr.accept(*this);
        ss << ")";
        return ss.str();
    }
};