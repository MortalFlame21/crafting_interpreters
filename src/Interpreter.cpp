#include <iostream>
#include <any>

#include "Lexer.h"
#include "Interpreter.h"
#include "Lox.h"
#include "Environment.h"

std::any Interpreter::visitBinary(const Binary& binary) {
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
        }
    }
    catch (const std::bad_any_cast& e)
    {
        std::cerr << "Error: std::any_cast: " << e.what() << "\n";
        return nullptr;
    }

    return nullptr;
}

std::any Interpreter::visitGrouping(const Grouping& grouping) {
    // would like to use some shared pointer here.
    return evaluate(grouping.m_expression.get());
}

std::any Interpreter::visitLiteral(const Literal& literal) {
    return literal.m_value;
}

std::any Interpreter::visitUnary(const Unary& unary) {
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
        };
    }
    catch (const std::bad_any_cast& e)
    {
        std::cerr << "Error: std::any_cast: " << e.what() << "\n";
        return nullptr;
    }

    return nullptr;
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
std::any Interpreter::visitExpressionStmt(const ExpressionStmt& stmt) {
    evaluate(stmt.m_expression.get());
    return {};
}

std::any Interpreter::visitPrintStmt(const PrintStmt& stmt) {
    std::any expr { evaluate(stmt.m_expression.get() )};
    std::cout << str(expr) << "\n";
    return {};
}

void Interpreter::execute(Statement* stmt) {
    stmt->accept(*this);
}

std::any Interpreter::visitVariable(const Variable& variable) {
    return "visitVariable";
}

std::any Interpreter::visitVariableStmt(const VariableStmt& stmt) {
    return "visitVariableStmt";
}