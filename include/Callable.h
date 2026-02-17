#pragma once

#include <any>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Statement.h"

class Interpreter;
class Environment;

class Callable {
public:
    Callable() {}
    virtual ~Callable() {}

    virtual std::any call (
        [[maybe_unused]] Interpreter& interpreter,
        [[maybe_unused]] std::vector<std::any> args
    ) = 0;
    virtual std::size_t arity() = 0;
    virtual std::string str() { return "<function>"; }
};

class ClockCallable : public Callable {
public:
    ClockCallable() {}
    virtual ~ClockCallable() {}

    std::any call(
        [[maybe_unused]] Interpreter& interpreter,
        [[maybe_unused]] std::vector<std::any> args
    ) override;
    std::size_t arity() override;
};

class FunctionCallable : public Callable {
public:
    FunctionCallable(
        FunctionStmt* declaration,
        std::shared_ptr<Environment> closure
    )
        : m_declaration { declaration }
        , m_closure { closure }
    { }

    FunctionCallable(const FunctionCallable&) = delete;
    FunctionCallable& operator=(const FunctionCallable&) = delete;

    std::any call(
        [[maybe_unused]] Interpreter& interpreter,
        [[maybe_unused]] std::vector<std::any> args
    ) override;
    std::size_t arity() override;
    virtual std::string str() override;
private:
    FunctionStmt* m_declaration;
    std::shared_ptr<Environment> m_closure;
};

// class used to be able to easily unwind the stack to return value
class ReturnStmtStackError : public std::runtime_error {
public:
    ReturnStmtStackError(std::any& value)
        : std::runtime_error("ReturnStmtStackError.")
        , m_value{ value }
    { };

    std::any m_value;
};