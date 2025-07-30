#pragma once

/*
the grammar

expression  -> <equality>;
equality    -> <comparision> != | == <comparision>;
comparision -> <term> > | >= | < | <= <term>;
term        -> <factor> - | + <factor>;
factor      -> <unary> / | * <unary>;
unary       -> (! | -) <unary> | <primary>;
primary     -> <NUMBER> | <STRING>
               | "true" | "false" | "nil"
               | "(" <expression> ")";

*/

#include <string>
#include <string_view>
#include <memory>
#include <stdexcept>

#include "Lexer.h"
#include "Expression.h"

class Parser {
public:
    Parser(std::vector<Token> tokens);
private:
    class ParseError : public std::runtime_error {
        ParseError(const std::string& error)
            : std::runtime_error{ error } { };
        friend class Parser;
    };

    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> equality();
    bool match(std::vector<Token::Type> types);
    bool check(Token::Type t);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();
    // using const std::string& to follow inheritance of
    // std::runtime_error.
    Token consume(Token::Type type, const std::string& msg);
    ParseError error(Token token, const std::string& msg);

    const std::vector<Token> m_tokens{};
    std::size_t m_current{};
};