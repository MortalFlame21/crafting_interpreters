#pragma once

#include <memory>
#include <any>

#include "Lexer.h"

class Visitor {
public:
    Visitor() = default;
    virtual ~Visitor();
	virtual std::any visitBinary() = 0;
	virtual std::any visitGrouping() = 0;
	virtual std::any visitUnary() = 0;
};

class Expression {
public:
    Expression() = default;
    virtual ~Expression();
	virtual std::any accept(Visitor& visitor);
};

class Binary {
public:
    Binary(Expression left, Expression right, Token operator_)
        : m_left{ std::make_unique<Expression>(left) }
        , m_right{ std::make_unique<Expression>(right)  }
        , m_operator{ operator_ }
    { }

    std::any accept(Visitor& visitor) {
        return visitor.visitBinary();
    }
private:
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
    Token m_operator;
};

class Grouping {
public:
    Grouping(Expression expression)
        : m_expression{ std::make_unique<Expression>(expression) }
    { }

    std::any accept(Visitor& visitor) {
        return visitor.visitGrouping();
    }
private:
    std::unique_ptr<Expression> m_expression;
};

class Unary {
public:
    Unary(Expression right, Token operator_)
        : m_right{ std::make_unique<Expression>(right) }
        , m_operator{ operator_ }
    { }

    std::any accept(Visitor& visitor) {
        return visitor.visitUnary();
    }
private:
    std::unique_ptr<Expression> m_right;
    Token m_operator;
};