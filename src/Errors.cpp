#include <string_view>
#include <iostream>

#include "Lexer.h"
#include "Errors.h"
#include "Lox.h"

namespace Errors {
    void errors(std::size_t line, std::string_view msg) {
        report(line, "", msg);
    }

    void errors([[maybe_unused]] Token token, [[maybe_unused]] std::string_view msg) {
        // if (token.m_type == Token::Type::EOF_TOKEN)
        //     report(token.m_line, " at end", msg);
        // else
        //     report(token.m_line, " at \'" + token.m_lexeme + "\'", msg);
    }

    void report(std::size_t line, std::string_view where, std::string_view msg) {
        std::cout << "[Line " << line << "] "
            << "Error " << where << ": "
            << msg << ".\n";
        Lox::hadError = true;
    }
}