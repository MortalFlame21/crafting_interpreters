#pragma once

/*
the grammar

expression  -> <equality>;
equality    -> <comparision> != | == <comparision>
comparision ->


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
private:
    const std::vector<Token> m_tokens{};
    std::size_t m_current{};
};