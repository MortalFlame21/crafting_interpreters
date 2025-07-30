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

#include "Lexer.h"
#include "Expression.h"

class Parser {
public:
    Parser(std::vector<Token> tokens);
    Expression& expression();
    Expression& equality();
    bool match(std::vector<Token::Type> types);
    Expression& comparison();
    bool check(Token::Type t);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Expression& term();
    Expression& factor();
    Expression& unary();
    Expression& primary();
private:
    const std::vector<Token> m_tokens{};
    std::size_t m_current{};
};