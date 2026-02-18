#include "LoxClass.h"

// LoxClass
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
    return "class<" + m_name + ">";
}

std::shared_ptr<FunctionCallable> LoxClass::findMethod(std::string name) {
    return (m_methods.find(name) != m_methods.end()) ? m_methods.find(name)->second : nullptr;
}

// LoxInstance
std::any LoxInstance::get(Token name) {
    if (m_fields.find(name.m_lexeme) != m_fields.end())
        return m_fields.find(name.m_lexeme)->second;

    if (auto method { m_class->findMethod(name.m_lexeme) }; method)
        return method->bind(this);

    throw Interpreter::RuntimeError(name, "Undefined property " + name.m_lexeme);
}

void LoxInstance::set(Token name, std::any& value) {
    m_fields.insert_or_assign(name.m_lexeme, value);
}