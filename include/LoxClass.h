#pragma once

#include <iostream>
#include <string>

class LoxClass {
public:
    LoxClass(std::string name)
        : m_name { name } { }

    std::ostream& operator<<(std::ostream& out) {
        return out << "class <" << m_name << ">";
    }

private:
    std::string m_name;
};