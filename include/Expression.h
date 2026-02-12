#pragma once

#include <memory>
#include <any>
#include <vector>

#include "Lexer.h"

class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assignment;
class Logical;
class Call;
class Get;

class Expression {
public:
    Expression() = default;
    virtual ~Expression() { };

    class Visitor {
    public:
        Visitor() = default;
        virtual ~Visitor() { };
        // do I need std::any here or std::string is enough?
        virtual std::any visitBinary(Binary& binary) = 0;
        virtual std::any visitGrouping(Grouping& grouping) = 0;
        virtual std::any visitLiteral(Literal& literal) = 0;
        virtual std::any visitUnary(Unary& unary) = 0;
        virtual std::any visitVariable(Variable& variable) = 0;
        virtual std::any visitAssignment(Assignment& Assignment) = 0;
        virtual std::any visitLogical(Logical& logical) = 0;
        virtual std::any visitCall(Call& call) = 0;
        virtual std::any visitGet(Get& get) = 0;
    };

    virtual std::any accept(Visitor& visitor) = 0;

    friend class AstPrinter;
    friend class Interpreter;
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

    friend class AstPrinter;
    friend class Interpreter;
    friend class Resolver;
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

    friend class AstPrinter;
    friend class Interpreter;
    friend class Resolver;
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

    friend class AstPrinter;
    friend class Interpreter;
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

    friend class AstPrinter;
    friend class Interpreter;
    friend class Resolver;
private:
    std::unique_ptr<Expression> m_right;
    Token m_operator;
};

class Variable : public Expression {
public:
    Variable(Token name)
        : m_name{ name }
    { }

    virtual ~Variable() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitVariable(*this);
    }

    friend class AstPrinter;
    friend class Interpreter;
    friend class Parser;
    friend class Resolver;
private:
    Token m_name;
};

class Assignment : public Expression {
public:
    Assignment(Token name, std::unique_ptr<Expression> value)
        : m_name{ name }, m_value{ std::move(value) }
    { }

    virtual ~Assignment() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitAssignment(*this);
    }

    friend class AstPrinter;
    friend class Interpreter;
    friend class Resolver;
private:
    Token m_name;
    std::unique_ptr<Expression> m_value;
};

class Logical : public Expression {
public:
    Logical (
        std::unique_ptr<Expression> left,
        std::unique_ptr<Expression> right,
        Token operator_
    )
        : m_left{ std::move(left) }
        , m_right{ std::move(right)  }
        , m_operator{ operator_ }
    { }

    virtual ~Logical() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitLogical(*this);
    }

    friend class AstPrinter;
    friend class Interpreter;
    friend class Resolver;
private:
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
    Token m_operator;
};

class Call : public Expression {
public:
    Call (
        std::unique_ptr<Expression> callee,
        Token parenthesis,
        std::vector<std::unique_ptr<Expression>> arguments
    )
        : m_callee{ std::move(callee) }
        , m_parenthesis{ parenthesis }
        , m_arguments{ std::move(arguments) }
    { }

    virtual ~Call() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitCall(*this);
    }

    friend class AstPrinter;
    friend class Interpreter;
    friend class Resolver;
private:
    std::unique_ptr<Expression> m_callee;
    Token m_parenthesis;
    std::vector<std::unique_ptr<Expression>> m_arguments;
};

class Get : public Expression {
public:
    Get (
        std::unique_ptr<Expression> object,
        Token name
    )
        : m_object{ std::move(object) }
        , m_name{ name }
    { }

    virtual ~Get() { };

    std::any accept(Visitor& visitor) override {
        return visitor.visitGet(*this);
    }

    friend class AstPrinter;
    friend class Interpreter;
    friend class Resolver;
private:
    std::unique_ptr<Expression> m_object;
    Token m_name;
};