#pragma once

#include <string>
#include <string_view>
#include <sstream>

#include "Expression.h"

using namespace std::string_view_literals;

class AstPrinter : public Visitor {
public:
    std::string_view print() {
        return "AstPrinter::print"sv;
    }

	std::any visitBinary() override {
        return "AstPrinter::visitBinary"sv;
    };

	std::any visitGrouping() override {
        return "AstPrinter::visitBinary"sv;
    };

	std::any visitUnary() override {
        return "AstPrinter::visitUnary"sv;
    };
private:
    std::string parenthesize(
        std::string_view name,
        std::vector<Expression> expressions
    ) {
        std::stringstream ss{};
        ss << "(" << name;
        for (auto& expr : expressions)
            ss << " " << std::any_cast<std::string_view>(expr.accept(*this));
        ss << ")";
        return ss.str();
    }
};