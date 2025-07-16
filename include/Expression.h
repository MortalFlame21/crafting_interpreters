#pragma once

#include "Lexer.h"

template<typename T>
class Visitor {
public:
    Visitor() = default;
    virtual ~Visitor();
	virtual T visitBinary() = 0;
	virtual T visitGrouping() = 0;
	virtual T visitUnary() = 0;
};

template<typename T>
class Expression {
public:
    Expression() = default;
    virtual ~Expression();
	virtual T accept(Visitor<T>& visitor);
};

template<typename T>
class Binary : public Expression<T> {
public:
    Binary(Expression<T> left, Expression<T> right, Token operator_)
    :   m_left{ left }, m_right{ right }, m_operator{ operator_ } {
    };

    T accept(Visitor<T>& visitor) {
        return visitor.visitBinary();
    }
private:
    Expression<T> m_left;
    Expression<T> m_right;
    Token m_operator;
};

template<typename T>
class Grouping : public Expression<T> {
public:
    Grouping(Expression<T> expression)
        : m_expression{ expression } {
    }

    T accept(Visitor<T>& visitor) {
        return visitor.visitGrouping();
    }
private:
    Expression<T> m_expression;
};

template<typename T>
class Unary : public Expression<T> {
public:
    Unary(Expression<T> right, Token operator_)
        : m_right{ right }, m_operator{ operator_ } {
    }

    T accept(Visitor<T>& visitor) {
        return visitor.visitUnary();
    }
private:
    Expression<T> m_right;
    Token m_operator;
};