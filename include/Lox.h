#pragma once

#include <string_view>

#include "Errors.h"

class Lox {
public:
    static void runFile(const std::string& file);
    static void runPrompt();
private:
    static inline bool hadError{};
    static void run(std::string_view source);

    friend void Errors::report(std::size_t line, std::string_view where, std::string_view msg);
};