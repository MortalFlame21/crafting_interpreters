#pragma once

#include <unordered_map>
#include <string>
#include <any>

class Token;
class Interpreter;

class Environment {
public:
    void define(const std::string& name, std::any& value);
    std::any get(Token name);
    void assign(Token name, std::any& value);
private:
    std::unordered_map<std::string, std::any> m_values {};
};