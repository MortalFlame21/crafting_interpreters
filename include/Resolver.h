#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <queue>

#include "Interpreter.h"
#include "Expression.h"
#include "Statement.h"

class Resolver : public Expression::Visitor, public Statement::Visitor {
public:
    Resolver(std::shared_ptr<Interpreter> interpreter)
        : m_interpreter{ interpreter } { }

    // expressions
	std::any visitBinary(Binary& binary) override;
	std::any visitGrouping(Grouping& grouping) override;
	std::any visitLiteral(Literal& literal) override;
	std::any visitUnary(Unary& unary) override;
	std::any visitVariable(Variable& variable) override;
	std::any visitAssignment(Assignment& assignment) override;
	std::any visitLogical(Logical& logical) override;
	std::any visitCall(Call& call) override;
    // statements
    std::any visitExpressionStmt(ExpressionStmt& stmt) override;
    std::any visitPrintStmt(PrintStmt& stmt) override;
    std::any visitVariableStmt(VariableStmt& stmt) override;
    std::any visitBlockStmt(BlockStmt& stmt) override;
    std::any visitIfStatement(IfStmt& stmt) override;
    std::any visitWhileStmt(WhileStmt& stmt) override;
    std::any visitFunctionStmt(FunctionStmt& stmt) override;
    std::any visitReturnStmt(ReturnStmt& stmt) override;
private:
    void resolve(std::vector<std::unique_ptr<Statement>> statements);
    void resolve(Statement* statement);
    void resolve(Expression* expression);
    void beginScope();
    void endScope();
    void define(Token token);
    void declare(Token token);
    void resolveLocal(Expression* expr, Token name);

    std::shared_ptr<Interpreter> m_interpreter;
    std::vector<std::unordered_map<std::string, bool>> m_scopes;
};