#pragma once

#include <memory>
#include <any>

#include "Lexer.h"

class Visitor {
public:
    Visitor() = default;
    virtual ~Visitor() { };
	virtual std::any visitBinary(const Binary& binary) = 0;
	virtual std::any visitGrouping(const Grouping& grouping) = 0;
	virtual std::any visitLiteral(const Literal& literal) = 0;
	virtual std::any visitUnary(const Unary& unary) = 0;
};

class Expression {
public:
    Expression() = default;
    virtual ~Expression() { };

    virtual std::any accept(Visitor& visitor) { return "Expression::accept"; };
};

class Binary : public Expression {
public:
    Binary (
        std::unique_ptr<Expression> left,
        std::unique_ptr<Expression> right,
        Token operator_
    )
        : m_left{ std::move(left) }
        , m_right{ std::move(right)  }
        , m_operator{ operator_ }
    { }

    virtual ~Binary() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitBinary(*this);
    }
private:
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
    Token m_operator;
};

class Grouping : public Expression {
public:
    Grouping(std::unique_ptr<Expression> expression)
        : m_expression{ std::move(expression) }
    { }

    virtual ~Grouping() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitGrouping(*this);
    }
private:
    std::unique_ptr<Expression> m_expression;
};

class Literal : public Expression {
public:
    Literal(std::any value) : m_value { value } {}

    virtual ~Literal() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitLiteral(*this);
    }
private:
    std::any m_value;
};

class Unary : public Expression {
public:
    Unary(std::unique_ptr<Expression> right, Token operator_)
        : m_right{ std::move(right) }
        , m_operator{ operator_ }
    { }

    virtual ~Unary() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitUnary(*this);
    }
private:
    std::unique_ptr<Expression> m_right;
    Token m_operator;
};