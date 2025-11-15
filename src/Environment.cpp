#include "Environment.h"
#include "Lexer.h"
#include "Interpreter.h"

void Environment::define(const std::string& name, std::any& value) {
    m_values.insert({ name, value });
}

std::any Environment::get(Token name) {
    if (m_values.find(name.m_lexeme) != m_values.end())
        return m_values.find(name.m_lexeme)->second;

    // walk up the chain to find variable with same name
    if (m_enclosing) return m_enclosing->get(name);

    throw Interpreter::RuntimeError(name, "Undefined variable " + name.m_lexeme);
}

void Environment::assign(Token name, std::any& value) {
    if (m_values.find(name.m_lexeme) != m_values.end()) {
        m_values.at(name.m_lexeme) = value;
        return;
    }

    // walk up the chain to find variable with same name
    if (m_enclosing) {
        m_enclosing->assign(name, value);
        return;
    }

    throw Interpreter::RuntimeError(name, "Undefined variable " + name.m_lexeme);
}