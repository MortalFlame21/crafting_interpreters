#include <any>

#include "Resolver.h"
#include "Errors.h"

std::any Resolver::visitBinary(Binary& binary) {
    resolve(binary.m_left.get());
    resolve(binary.m_right.get());
    return {};
}

std::any Resolver::visitGrouping(Grouping& grouping) {
    resolve(grouping.m_expression.get());
    return {};
}

std::any Resolver::visitLiteral([[maybe_unused]] Literal& literal) {
    return {};
}

std::any Resolver::visitUnary(Unary& unary) {
    resolve(unary.m_right.get());
    return {};
}

std::any Resolver::visitVariable(Variable& variable) {
    if (m_scopes.empty() && !m_scopes.front().at(variable.m_name.m_lexeme)) {
        Errors::errors(variable.m_name, "Can't read local variable in its own initialiser.");
    }
    resolveLocal(&variable, variable.m_name);
    return {};
}

std::any Resolver::visitAssignment(Assignment& assignment) {
    resolve(assignment.m_value.get());
    resolveLocal(&assignment, assignment.m_name);
    return {};
}

std::any Resolver::visitLogical(Logical& logical) {
    resolve(logical.m_left.get());
    resolve(logical.m_right.get());
    return {};
}

std::any Resolver::visitCall(Call& call) {
    resolve(call.m_callee.get());
    for (auto& a : call.m_arguments)
        resolve(a.get());
    return {};
}

std::any Resolver::visitExpressionStmt(ExpressionStmt& stmt) {
    resolve(stmt.m_expression.get());
    return {};
}

std::any Resolver::visitPrintStmt(PrintStmt& stmt) {
    resolve(stmt.m_expression.get());
    return {};
}

std::any Resolver::visitVariableStmt(VariableStmt& stmt) {
    declare(stmt.m_name);
    if (stmt.m_expression) resolve(stmt.m_expression.get());
    define(stmt.m_name);
    return {};
}

std::any Resolver::visitBlockStmt(BlockStmt& stmt) {
    beginScope();
    resolve(std::move(stmt.m_statements));
    endScope();
    return {};
}

std::any Resolver::visitIfStatement(IfStmt& stmt) {
    resolve(stmt.m_condition.get());
    resolve(stmt.m_thenBranch.get());
    if (stmt.m_thenBranch) resolve(stmt.m_elseBranch.get());
    return {};
}

std::any Resolver::visitWhileStmt(WhileStmt& stmt) {
    resolve(stmt.m_condition.get());
    resolve(stmt.m_body.get());
    return {};
}

std::any Resolver::visitFunctionStmt(FunctionStmt& stmt) {
    declare(stmt.m_name);
    define(stmt.m_name);
    resolveFunction(&stmt);
    return {};
}

std::any Resolver::visitReturnStmt(ReturnStmt& stmt) {
    if (!stmt.m_value)
        resolve(stmt.m_value.get());
    return {};
}

void Resolver::resolve(std::vector<std::unique_ptr<Statement>> statements) {
    for (auto& s : statements) {
        resolve(s.get());
    }
}

void Resolver::resolve(Statement* statement) {
    statement->accept(*this);
}

void Resolver::resolve(Expression* expression) {
    expression->accept(*this);
}

void Resolver::beginScope() {
    // create a new unordered_map (a new scope)
    m_scopes.push_back({});
}

void Resolver::endScope() {
    m_scopes.erase(m_scopes.begin());
}

void Resolver::declare(Token token) {
    if (m_scopes.empty()) return;

    auto scope { m_scopes.front() };
    scope.insert({ token.m_lexeme, false });
}

void Resolver::define(Token token) {
    if (m_scopes.empty()) return;
    m_scopes.front().insert({ token.m_lexeme, true });
}

void Resolver::resolveLocal(Expression* expr, Token name) {
    // use std::ssize() next time (C++20)
    auto sz { static_cast<ptrdiff_t>(m_scopes.size()) };
    for (auto i { sz }; i >= 0; --i) {
        auto j { static_cast<size_t>(i) };
        if (m_scopes[j].find(name.m_lexeme) != m_scopes[j].end()) {
            m_interpreter->resolve(expr, static_cast<int>(m_scopes.size() - 1 - j));
        }
    }
}

void Resolver::resolveFunction(FunctionStmt* function) {
    beginScope();
    for (auto& p : function->m_params) {
        declare(p);
        define(p);
    }
    resolve(std::move(function->m_body));
    endScope();
}