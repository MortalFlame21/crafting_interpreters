#pragma once

#include <string_view>

#include "Errors.h"
#include "Interpreter.h"

class Lox {
public:
    static void runFile(const std::string& file);
    static void runPrompt();
    static void runtimeError(Interpreter::RuntimeError& error);
private:
    static inline bool hadError{};
    static inline bool hadRuntimeError{};

    static void run(std::string_view source);

    friend void Errors::report(std::size_t line, std::string_view where, std::string_view msg);
};