#pragma once

#include <string>

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
};