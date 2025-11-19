#pragma once

#include <memory>
#include <any>

#include "Expression.h"

class ExpressionStmt;
class PrintStmt;
class VariableStmt;
class BlockStmt;
class IfStmt;
class While;

class Statement {
public:
    Statement() = default;
    virtual ~Statement() { };

    class Visitor {
    public:
        Visitor() = default;
        virtual ~Visitor() { };
        // do I need std::any here or std::string is enough?
        virtual std::any visitExpressionStmt(ExpressionStmt& stmt) = 0;
        virtual std::any visitPrintStmt(PrintStmt& stmt) = 0;
        virtual std::any visitVariableStmt(VariableStmt& stmt) = 0;
        virtual std::any visitBlockStmt(BlockStmt& stmt) = 0;
        virtual std::any visitIfStatement(IfStmt& stmt) = 0;
        virtual std::any visitWhileStatement(While& while_) = 0;
    };

    virtual std::any accept(Visitor& visitor) = 0;

    friend class AstPrinter;
    friend class Interpreter;
};

class ExpressionStmt : public Statement {
public:
    ExpressionStmt(std::unique_ptr<Expression> expression)
        : m_expression{ std::move(expression) }
    { }
    virtual ~ExpressionStmt() { }

    std::any accept(Visitor& visitor) {
        return visitor.visitExpressionStmt(*this);
    }

    friend class Interpreter;
private:
    std::unique_ptr<Expression> m_expression;
};

class PrintStmt : public Statement {
public:
    PrintStmt(std::unique_ptr<Expression> expression)
        : m_expression{ std::move(expression) }
    { }
    virtual ~PrintStmt() { }

    std::any accept(Visitor& visitor) {
        return visitor.visitPrintStmt(*this);
    }

    friend class Interpreter;
private:
    std::unique_ptr<Expression> m_expression;
};

class VariableStmt : public Statement {
public:
    VariableStmt(Token name, std::unique_ptr<Expression> expression)
        : m_name { name }, m_expression{ std::move(expression) }
    { }
    virtual ~VariableStmt() { }

    std::any accept(Visitor& visitor) {
        return visitor.visitVariableStmt(*this);
    }

    friend class Interpreter;
private:
    Token m_name;
    std::unique_ptr<Expression> m_expression;
};

class BlockStmt : public Statement {
public:
    BlockStmt(std::vector<std::unique_ptr<Statement>> statements)
        : m_statements{ std::move(statements) } { }
    virtual ~BlockStmt() { }

    std::any accept(Visitor& visitor) {
        return visitor.visitBlockStmt(*this);
    }

    friend class Interpreter;
private:
    std::vector<std::unique_ptr<Statement>> m_statements;
};

class IfStmt : public Statement {
public:
    IfStmt(
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Statement> thenBranch,
        std::unique_ptr<Statement> elseBranch
    )
    : m_condition { std::move(condition) }
    , m_thenBranch { std::move(thenBranch) }
    , m_elseBranch { std::move(elseBranch) }
    { }

    virtual ~IfStmt() { }

    std::any accept(Visitor& visitor) {
        return visitor.visitIfStatement(*this);
    }

    friend class Interpreter;
private:
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Statement> m_thenBranch;
    std::unique_ptr<Statement> m_elseBranch;
};

class While : public Statement {
public:
    While (
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Statement> body
    )
        : m_condition{ std::move(condition) }
        , m_body{ std::move(body) }
    { }

    virtual ~While() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitWhile(*this);
    }

    friend class AstPrinter;
    friend class Interpreter;
private:
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Statement> m_body;
};