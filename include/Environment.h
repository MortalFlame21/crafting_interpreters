#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <any>

class Token;
class Interpreter;

class Environment {
public:
    Environment() { }
    Environment(std::unique_ptr<Environment> enclosing)
        : m_enclosing { std::move(enclosing) }
    { }

    void define(const std::string& name, std::any& value);
    std::any get(Token name);
    void assign(Token name, std::any& value);
private:
    std::unordered_map<std::string, std::any> m_values {};
    std::unique_ptr<Environment> m_enclosing {};
};