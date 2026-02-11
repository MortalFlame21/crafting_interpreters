#include "LoxClass.h"

std::any LoxClass::call (
    [[maybe_unused]] Interpreter& interpreter,
    [[maybe_unused]] std::vector<std::any> args
) {
    auto instance = std::make_shared<LoxInstance>(this);
    return instance;
}

std::size_t LoxClass::arity() {
    return 0;
}

std::string LoxClass::str() {
    return "class <" + m_name + ">";
}