#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <any>

#include "Interpreter.h"
#include "Callable.h"

class LoxInstance;

class LoxClass : public Callable {
public:
    LoxClass(std::string name)
        : m_name { name } { }

    std::ostream& operator<<(std::ostream& out) {
        return out << "class<" << m_name << ">";
    }

    std::any call (
        [[maybe_unused]] Interpreter& interpreter,
        [[maybe_unused]] std::vector<std::any> args
    ) override;
    std::size_t arity() override;
    std::string str() override;

    friend class LoxInstance;
private:
    std::string m_name;
};

class LoxInstance {
public:
    LoxInstance(LoxClass* class_)
        : m_class { class_ } { }

    virtual ~LoxInstance() { };

    // explicitly define the move ctor and move assignment ctor.
    LoxInstance(LoxInstance&) = default;
    LoxInstance& operator=(LoxInstance&&) = default;

    // disallow copy ctor and copy assignment ctor.
    LoxInstance(const LoxInstance&) = delete;
    LoxInstance& operator=(const LoxInstance&) = delete;

    std::string str() {
        return "classInstance<" + m_class->m_name + ">";
    }

    std::any get(Token name) {
        if (m_fields.find(name.m_lexeme) != m_fields.end())
            return m_fields.find(name.m_lexeme)->second;
        throw Interpreter::RuntimeError(name, "Undefined property " + name.m_lexeme);
    }
private:
    LoxClass* m_class {};
    std::unordered_map<std::string, std::any> m_fields {};
};