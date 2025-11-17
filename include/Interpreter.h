#pragma once

#include <iostream>
#include <stdexcept>
#include <memory>
#include <any>

#include "Lexer.h"
#include "Expression.h"
#include "Statement.h"
#include "Environment.h"

class Interpreter : public Expression::Visitor, public Statement::Visitor {
public:
    class RuntimeError : public std::runtime_error {
    public:
        RuntimeError(Token token, const std::string& error)
            : std::runtime_error{ error }, m_token{ token } { };

        Token m_token;
    };

    // expressions
	std::any visitBinary(const Binary& binary) override;
	std::any visitGrouping(const Grouping& grouping) override;
	std::any visitLiteral(const Literal& literal) override;
	std::any visitUnary(const Unary& unary) override;
	std::any visitVariable(const Variable& variable) override;
	std::any visitAssignment(const Assignment& assignment) override;
    // statements
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitPrintStmt(const PrintStmt& stmt) override;
    std::any visitVariableStmt(const VariableStmt& stmt) override;
    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitIfStatement(const IfStmt& stmt) override;

    void interpret(std::vector<std::unique_ptr<Statement>> statements);
    void execute(Statement* stmt);
    void executeBlock (
        const std::vector<std::unique_ptr<Statement>>& statements,
        std::shared_ptr<Environment> environment
    );
private:
    std::any evaluate(Expression* expression);
    bool isTruthy(std::any object);
    bool isEqual(std::any left, std::any right);
    void checkNumberOperand(Token operator_, std::any operand);
    void checkNumberOperands(Token operator_, std::any left, std::any right);
    std::string str(std::any object);

    std::shared_ptr<Environment> m_environment { std::make_shared<Environment>() };
};