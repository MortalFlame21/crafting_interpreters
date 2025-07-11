#include <string_view>
#include <iostream>

#include "Errors.h"
#include "Lox.h"

namespace Errors {
    void errors(std::size_t line, std::string_view msg) {
        report(line, "", msg);
    }

    void report(std::size_t line, std::string_view where, std::string_view msg) {
        std::cout << "[Line " << line << "] "
            << "Error " << where << ": "
            << msg << ".\n";
        Lox::hadError = true;
    }
}