#pragma once

#include <iostream>
#include <string>

#include "Callable.h"

class LoxClass : public Callable {
public:
    LoxClass(std::string name)
        : m_name { name } { }

    std::ostream& operator<<(std::ostream& out) {
        return out << "class <" << m_name << ">";
    }

    std::any call (
        [[maybe_unused]] Interpreter& interpreter,
        [[maybe_unused]] std::vector<std::any> args
    ) override;
    std::size_t arity() override;
    std::string str() override;
private:
    std::string m_name;
};