#pragma once

#include <string_view>

#include "Lexer.h"

namespace Errors {
    void errors(std::size_t line, std::string_view msg);
    void errors(Token token, std::string_view msg);
    void report(std::size_t line, std::string_view where, std::string_view msg);
}