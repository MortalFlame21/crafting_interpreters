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
#include <vector>
#include <memory>

#include "Lexer.h"
#include "Expression.h"

class Parser {
public:
    Parser(std::vector<Token> tokens);
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
private:
    const std::vector<Token> m_tokens{};
    std::size_t m_current{};
};