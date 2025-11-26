#include <iostream>
#include <any>

#include "Lexer.h"
#include "Interpreter.h"
#include "Lox.h"
#include "Environment.h"
#include "Callable.h"

std::any Interpreter::visitBinary(Binary& binary) {
    std::any left { evaluate(binary.m_left.get()) };
    std::any right { evaluate(binary.m_right.get()) };

    try {
        // might need to fix probably does not cast properly
        switch (binary.m_operator.m_type) {
        // comparision
        case Token::Type::GREATER:
            checkNumberOperands(binary.m_operator, left, right);
            return std::any_cast<double>(left) > std::any_cast<double>(right);
        case Token::Type::GREATER_EQUAL:
            checkNumberOperands(binary.m_operator, left, right);
            return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case Token::Type::LESS:
            checkNumberOperands(binary.m_operator, left, right);
            return std::any_cast<double>(left) < std::any_cast<double>(right);
        case Token::Type::LESS_EQUAL:
            checkNumberOperands(binary.m_operator, left, right);
            return std::any_cast<double>(left) <= std::any_cast<double>(right);
        case Token::Type::EQUAL_EQUAL:
            return isEqual(left, right);
        case Token::Type::EXCLAIM_EQUAL:
            return !isEqual(left, right);
        // arithmetic
        case Token::Type::BACK_SLASH:
            checkNumberOperands(binary.m_operator, left, right);
            if (std::abs(std::any_cast<double>(right)) == 0.0)
                throw RuntimeError (binary.m_operator, "Right operand. Divide by zero");
            return std::any_cast<double>(left) / std::any_cast<double>(right);
        case Token::Type::ASTERISK:
            checkNumberOperands(binary.m_operator, left, right);
            return std::any_cast<double>(left) * std::any_cast<double>(right);
        case Token::Type::MINUS:
            checkNumberOperands(binary.m_operator, left, right);
            return std::any_cast<double>(left) - std::any_cast<double>(right);
        case Token::Type::PLUS:
            // both support adding and string concatenation
            if (left.type() == typeid(double) && right.type() == typeid(double))
                return std::any_cast<double>(left) + std::any_cast<double>(right);

            if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
                return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);

            // optional concatenate string and number.

            throw new RuntimeError (
                binary.m_operator, "Operands must be two numbers or two strings"
            );
        default:
            return nullptr;
        }
    }
    catch (const std::bad_any_cast& e)
    {
        std::cerr << "Error: std::any_cast: " << e.what() << "\n";
        return nullptr;
    }
}

std::any Interpreter::visitGrouping(Grouping& grouping) {
    // would like to use some shared pointer here.
    return evaluate(grouping.m_expression.get());
}

std::any Interpreter::visitLiteral(Literal& literal) {
    return literal.m_value;
}

std::any Interpreter::visitUnary(Unary& unary) {
    // would like to use some shared pointer here.
    std::any right { evaluate(unary.m_right.get()) };

    try {
        // might need to fix probably does not cast properly
        switch (unary.m_operator.m_type) {
        case Token::Type::EXCLAIM:
            return !isTruthy(right);
        case Token::Type::MINUS:
            checkNumberOperand(unary.m_operator, right);
            return std::any_cast<double>(right);
        default:
            return nullptr;
        };
    }
    catch (const std::bad_any_cast& e)
    {
        std::cerr << "Error: std::any_cast: " << e.what() << "\n";
        return nullptr;
    }
}

std::any Interpreter::evaluate(Expression* expression) {
    return expression->accept(*this);
}

bool Interpreter::isTruthy(std::any object) {
    if (!object.has_value()) return false;
    if (object.type() == typeid(bool)) return std::any_cast<bool>(object);
    return true;
}

bool Interpreter::isEqual(std::any left, std::any right) {
    if (!(left.has_value() && right.has_value())) return true;
    if (!left.has_value()) return false;

    if (left.type() == typeid(double) && right.type() == typeid(double))
        return std::any_cast<double>(left) == std::any_cast<double>(right);

    if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
        return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);

    // likely comparing number and string type. just return false.
    return false;
}

void Interpreter::checkNumberOperand(Token operator_, std::any operand) {
    if (operand.type() == typeid(double))
        return;
    throw RuntimeError(operator_, "Operand must be a number");
}

void Interpreter::checkNumberOperands (
    Token operator_, std::any left, std::any right
) {
    if (left.type() == typeid(double) && right.type() == typeid(double))
        return;
    throw RuntimeError(operator_, "Operand must be numbers");
}

void Interpreter::interpret(std::vector<std::unique_ptr<Statement>> statements) {
    try {
        for (auto& stmt : statements) {
            execute(stmt.get());
        }
    }
    catch (RuntimeError& e) {
        Lox::runtimeError(e);
    }
    catch (...) {
        std::cout << "Unexpected error occurred.\n";
    }
}

std::string Interpreter::str(std::any object) {
    if (!object.has_value())
        return "nil";

    if (object.type() == typeid(double)) {
        auto double_str { std::to_string(std::any_cast<double>(object)) };

        // ends with ".000000"? remove the decimal place -- (std::setprecision is default to 6 decimals)
        if (double_str.substr(double_str.size() - 7, 7) == ".000000")
           double_str = double_str.substr(0, double_str.size() - 7);

        return double_str;
    }
    else if (object.type() == typeid(bool)) {
        return std::any_cast<bool>(object) ? "true" : "false";
    }

    // most likely a string??
    return std::any_cast<std::string>(object);
}
std::any Interpreter::visitExpressionStmt(ExpressionStmt& stmt) {
    evaluate(stmt.m_expression.get());
    return {};
}

std::any Interpreter::visitPrintStmt(PrintStmt& stmt) {
    std::any expr { evaluate(stmt.m_expression.get() )};
    std::cout << str(expr) << "\n";
    return {};
}

void Interpreter::execute(Statement* stmt) {
    stmt->accept(*this);
}

std::any Interpreter::visitVariable(Variable& variable) {
    return m_environment->get(variable.m_name);
}

std::any Interpreter::visitVariableStmt(VariableStmt& stmt) {
    std::any value {};
    if (stmt.m_expression)
        value = evaluate(stmt.m_expression.get());

    m_environment->define(stmt.m_name.m_lexeme, value);
    return {};
}

std::any Interpreter::visitAssignment(Assignment& assignment) {
    std::any value { evaluate(assignment.m_value.get()) };
    if (m_environment)
        m_environment->assign(assignment.m_name, value);
    return value;
}

std::any Interpreter::visitBlockStmt(BlockStmt& stmt) {
    executeBlock (
        stmt.m_statements,
        std::make_shared<Environment>(m_environment)
    );
    return {};
}

void Interpreter::executeBlock (
    const std::vector<std::unique_ptr<Statement>>& statements,
    std::shared_ptr<Environment> environment
) {
    auto parent_env { m_environment };
    try {
        // use current environment
        m_environment = environment;

        for (auto& s : statements) {
            execute(s.get());
        }
    }
    // this is so wack...
    catch (...) {
        m_environment = parent_env;
        throw;
    }
    m_environment = parent_env;
}

std::any Interpreter::visitIfStatement(IfStmt& stmt) {
    if (isTruthy(evaluate(stmt.m_condition.get())))
        execute(stmt.m_thenBranch.get());
    else if (stmt.m_elseBranch)
        execute(stmt.m_elseBranch.get());
    return {};
}

std::any Interpreter::visitLogical(Logical& logical) {
    auto left { evaluate(logical.m_left.get()) };

    if (logical.m_operator.m_type == Token::Type::OR) {
        if (isTruthy(left))
            return left;
    }
    else {
        if (!isTruthy(left))
            return left;
    }

    return evaluate(logical.m_right.get());
}

std::any Interpreter::visitWhileStmt(WhileStmt& stmt) {
    while (isTruthy(evaluate(stmt.m_condition.get()))) {
        execute(stmt.m_body.get());
    }
    return {};
}

std::any Interpreter::visitCall(Call& call) {
    auto callee { evaluate(call.m_callee.get()) };

    std::vector<std::any> args {};
    for (auto& arg : call.m_arguments) {
        args.push_back(evaluate(arg.get()));
    }

    if (callee.type() != typeid(std::shared_ptr<Callable>)) {
        throw RuntimeError (
            call.m_parenthesis, "Can only call functions and classes"
        );
    }

    auto function { std::any_cast<std::shared_ptr<Callable>>(callee) };

    if (args.size() != function->arity()) {
        throw RuntimeError (
            call.m_parenthesis,
            "Expected " + std::to_string(function->arity()) +
            "arguments but got " + std::to_string(args.size()) + "arguments"
        );
    }

    return function->call(*this, args);
}