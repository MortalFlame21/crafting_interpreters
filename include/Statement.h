#pragma once

#include <memory>
#include <any>

#include "Expression.h"

class ExpressionStmt;
class PrintStmt;

class Statement {
public:
    Statement() = default;
    virtual ~Statement() { };

    class Visitor {
    public:
        Visitor() = default;
        virtual ~Visitor() { };
        // do I need std::any here or std::string is enough?
        virtual std::any visitExpressionStmt(const ExpressionStmt& stmt) = 0;
        virtual std::any visitPrintStmt(const PrintStmt& stmt) = 0;
    };

    virtual std::any accept(Visitor& visitor) = 0;

    friend class AstPrinter;
    friend class Interpreter;
};

class ExpressionStmt : public Statement {
public:
    ExpressionStmt() { }
    virtual ~ExpressionStmt() { }

    std::any accept(Visitor& visitor) {
        return visitor.visitExpressionStmt(*this);
    }
private:
    std::unique_ptr<Expression> expression;
};

class PrintStmt : public Statement {
public:
    PrintStmt() { }
    virtual ~PrintStmt() { }

    std::any accept(Visitor& visitor) {
        return visitor.visitPrintStmt(*this);
    }
private:
    std::unique_ptr<Expression> expression;
};
