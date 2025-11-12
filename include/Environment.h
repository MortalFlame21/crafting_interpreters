#pragma once

#include <unordered_map>
#include <string_view>
#include <any>

class Token;
class Interpreter;

class Environment {
private:
    void define(std::string_view name, std::any& value);

    std::any get(Token name);

    std::unordered_map<std::string_view, std::any> m_values {};
};