#pragma once

#include <iostream>
#include <stdexcept>
#include <memory>
#include <any>

#include "Lexer.h"
#include "Expression.h"
#include "Statement.h"
#include "Environment.h"
#include "Callable.h"

class LoxInstance;

class Interpreter : public Expression::Visitor, public Statement::Visitor {
public:
    class RuntimeError : public std::runtime_error {
    friend class LoxInstance;
    public:
        RuntimeError(Token token, const std::string& error)
            : std::runtime_error{ error }, m_token{ token } { };

        Token m_token;
    };

    Interpreter()
        : m_globals { std::make_shared<Environment>() }
        , m_environment { m_globals } {
        std::shared_ptr<Callable> clock { std::make_shared<ClockCallable>() };
        m_globals->define("clock", clock);
    }

    // expressions
	std::any visitBinary(Binary& binary) override;
	std::any visitGrouping(Grouping& grouping) override;
	std::any visitLiteral(Literal& literal) override;
	std::any visitUnary(Unary& unary) override;
	std::any visitVariable(Variable& variable) override;
	std::any visitAssignment(Assignment& assignment) override;
	std::any visitLogical(Logical& logical) override;
	std::any visitCall(Call& call) override;
	std::any visitGet(Get& get) override;
    // statements
    std::any visitExpressionStmt(ExpressionStmt& stmt) override;
    std::any visitPrintStmt(PrintStmt& stmt) override;
    std::any visitVariableStmt(VariableStmt& stmt) override;
    std::any visitBlockStmt(BlockStmt& stmt) override;
    std::any visitIfStatement(IfStmt& stmt) override;
    std::any visitWhileStmt(WhileStmt& stmt) override;
    std::any visitFunctionStmt(FunctionStmt& stmt) override;
    std::any visitReturnStmt(ReturnStmt& stmt) override;
    std::any visitClassStmt(ClassStmt& stmt) override;

    void interpret(const std::vector<std::unique_ptr<Statement>>& statements);
    void execute(Statement* stmt);
    void resolve(Expression* expr, int depth);
    std::any lookUpVariable(Token name, Expression* expr);
    void executeBlock (
        const std::vector<std::unique_ptr<Statement>>& statements,
        std::shared_ptr<Environment> environment
    );

    friend class FunctionCallable;
private:
    std::any evaluate(Expression* expression);
    bool isTruthy(std::any object);
    bool isEqual(std::any left, std::any right);
    void checkNumberOperand(Token operator_, std::any operand);
    void checkNumberOperands(Token operator_, std::any left, std::any right);
    std::string str(std::any object);

    std::shared_ptr<Environment> m_globals {};
    std::shared_ptr<Environment> m_environment {};
    std::unordered_map<Expression*, int> m_locals {};
};