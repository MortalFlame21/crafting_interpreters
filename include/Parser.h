#pragma once

/*
the grammar

program         -> <declaration>;
declaration     -> <var_declaration> | <statement>;
var_declaration -> "var" <IDENTIFIER> "=" <expression> ";";
statement       -> <expression_stmt> ";" | <print_stmt> ";"
                    | <block_stmt> | <if_stmt>;
if_stmt         -> "if" "(" <expression_stmt> ")"
                    <statement> "else" <statement>;
block_stmt      -> "{" <declaration> "}";
expression_stmt -> <expression> ";";
print_stmt      -> "print" <expression> ";";
expression      -> <assignment>;
assignment      -> <IDENTIFIER> "=" <assignment>
                    | <equality>
equality        -> <comparision> != | == <comparision>;
comparision     -> <term> > | >= | < | <= <term>;
term            -> <factor> - | + <factor>;
factor          -> <unary> / | * <unary>;
unary           -> (! | -) <unary> | <primary>;
primary         -> <NUMBER> | <STRING>
                    | "true" | "false" | "nil"
                    | "(" <expression> ")"
                    | <IDENTIFIER>;
*/

#include <string>
#include <string_view>
#include <memory>
#include <stdexcept>

#include "Lexer.h"
#include "Expression.h"
#include "Statement.h"

class Parser {
public:
    Parser(std::vector<Token> tokens);

    std::vector<std::unique_ptr<Statement>> parse();
private:
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& error)
            : std::runtime_error{ error } { };
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
    void synchronise();
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> expressionStatement();
    std::unique_ptr<Statement> printStatement();
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<Statement> varDeclaration();
    std::unique_ptr<Expression> assignment();
    std::vector<std::unique_ptr<Statement>> block();
    std::unique_ptr<Statement> ifStatement();

    const std::vector<Token> m_tokens{};
    std::size_t m_current{};
};