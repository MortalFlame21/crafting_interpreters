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
    Environment(std::shared_ptr<Environment> enclosing)
        : m_enclosing { enclosing }
    { }

    void define(const std::string& name, const std::any& value);
    std::any get(Token name);
    std::any getAt(int distance, std::string name);
    void assignAt(int distance, Token name, std::any& value);
    Environment* ancestor(int distance);
    void assign(Token name, const std::any& value);
private:
    std::unordered_map<std::string, std::any> m_values {};
    std::shared_ptr<Environment> m_enclosing {};
};