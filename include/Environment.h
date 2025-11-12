#pragma once

#include <unordered_map>
#include <string_view>
#include <any>

#include "Interpreter.h"

class Environment {
private:
    void define(std::string_view name, std::any& value) {
        m_values.insert({ name, value });
    }

    std::any get(Token name) {
        if (m_values.find(name.m_lexeme) != m_values.end())
            return m_values.find(name.m_lexeme);
        throw Interpreter::RuntimeError(name, "Undefined variable " + name.m_lexeme);
    }

    std::unordered_map<std::string_view, std::any> m_values {};
};