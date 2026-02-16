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
    // declared but not defined
    if (!m_scopes.empty()
        && m_scopes.back().find(variable.m_name.m_lexeme) != m_scopes.back().end()
        && !m_scopes.back().at(variable.m_name.m_lexeme)
    ) {
        Errors::errors(variable.m_name,
            "Can't read local variable in its own initialiser.");
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

std::any Resolver::visitGet(Get& get) {
    resolve(get.m_object.get());
    return {};
}

std::any Resolver::visitSet(Set& set) {
    resolve(set.m_value.get());
    resolve(set.m_object.get());
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
    resolve(stmt.m_statements);
    endScope();
    return {};
}

std::any Resolver::visitIfStatement(IfStmt& stmt) {
    resolve(stmt.m_condition.get());
    resolve(stmt.m_thenBranch.get());
    if (stmt.m_elseBranch) resolve(stmt.m_elseBranch.get());
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
    resolveFunction(&stmt, FunctionType::FUNCTION);
    return {};
}

std::any Resolver::visitReturnStmt(ReturnStmt& stmt) {
    if (m_currentFunction == FunctionType::NONE)
        Errors::errors(stmt.m_keyword, "Can't return from top-level code.");
    if (stmt.m_value)
        resolve(stmt.m_value.get());
    return {};
}

void Resolver::resolve(const std::vector<std::unique_ptr<Statement>>& statements) {
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
    // remove that scope (at the end)
    m_scopes.pop_back();
}

void Resolver::declare(Token token) {
    // global
    if (m_scopes.empty()) return;

    // if multiple variables, throw error.
    if (auto& scope { m_scopes.back() }; scope.find(token.m_lexeme) != scope.end())
        Errors::errors(token, "Already a variable with this name in this scope.");
    else
        scope.insert({ token.m_lexeme, false });
}

void Resolver::define(Token token) {
    if (m_scopes.empty()) return;
    m_scopes.back().insert_or_assign(token.m_lexeme, true);
}

void Resolver::resolveLocal(Expression* expr, Token name) {
    // use std::ssize() next time (C++20)
    auto sz { static_cast<ptrdiff_t>(m_scopes.size()) };
    for (auto i { sz - 1 }; i >= 0; --i) {
        auto j { static_cast<size_t>(i) };
        if (m_scopes[j].find(name.m_lexeme) != m_scopes[j].end()) {
            m_interpreter->resolve(expr, static_cast<int>(m_scopes.size() - 1 - j));
            return;
        }
    }
    // if not found it is a global
};

void Resolver::resolveFunction(FunctionStmt* function, FunctionType type) {
    auto enclosingFunction { m_currentFunction };
    m_currentFunction = type;

    beginScope();
    for (auto& p : function->m_params) {
        declare(p);
        define(p);
    }
    resolve(function->m_body);
    endScope();

    m_currentFunction = enclosingFunction;
}

std::any Resolver::visitClassStmt(ClassStmt& stmt) {
    declare(stmt.m_name);
    define(stmt.m_name);

    for (auto& m : stmt.m_methods) {
        resolveFunction(m, FunctionType::METHOD);
    }

    return {};
}