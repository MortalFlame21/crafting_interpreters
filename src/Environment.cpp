#include "Environment.h"
#include "Lexer.h"
#include "Interpreter.h"

void Environment::define(const std::string& name, std::any& value) {
    m_values.insert({ name, value });
}

std::any Environment::get(Token name) {
    if (m_values.find(name.m_lexeme) != m_values.end())
        return m_values.find(name.m_lexeme)->second;
    throw Interpreter::RuntimeError(name, "Undefined variable " + name.m_lexeme);
}