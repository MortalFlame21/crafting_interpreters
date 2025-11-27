#pragma once

#include <any>
#include <vector>

class Interpreter;

class Callable {
public:
    Callable() {}
    virtual ~Callable() {}

    virtual std::any call (
        [[maybe_unused]] Interpreter interpreter,
        [[maybe_unused]] std::vector<std::any> args
    ) = 0;
    virtual std::size_t arity() = 0;
};

class ClockCallable : public Callable {
public:
    ClockCallable() {}
    virtual ~ClockCallable() {}

    std::any call(
        [[maybe_unused]] Interpreter interpreter,
        [[maybe_unused]] std::vector<std::any> args
    ) override;
    std::size_t arity() override;
};