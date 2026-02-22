#include "Environment.h"
#include "Lexer.h"
#include "Interpreter.h"

void Environment::define(const std::string& name, const std::any& value) {
    m_values.insert_or_assign(name, value);
}

std::any Environment::get(Token name) {
    if (m_values.find(name.m_lexeme) != m_values.end())
        return m_values.find(name.m_lexeme)->second;

    // walk up the chain to find variable with same name
    if (m_enclosing) return m_enclosing->get(name);

    throw Interpreter::RuntimeError(name, "Undefined variable " + name.m_lexeme);
}

std::any Environment::getAt(int distance, std::string name) {
    return ancestor(distance)->m_values.find(name)->second;
}

void Environment::assignAt(int distance, Token name, std::any& value) {
    ancestor(distance)->m_values.insert_or_assign(name.m_lexeme, value);
}

Environment* Environment::ancestor(int distance) {
    auto env { this };
    for (int i {}; i < distance; ++i)
        env = env->m_enclosing.get();
    return env;
}

void Environment::assign(Token name, const std::any& value) {
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