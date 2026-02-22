#include <chrono>

#include "Callable.h"
#include "Interpreter.h"
#include "LoxClass.h"

// Begin ClockCallable

// get time in seconds? (double?) since epoch.
std::any ClockCallable::call (
    [[maybe_unused]] Interpreter& interpreter,
    [[maybe_unused]] std::vector<std::any> args
) {
    using Clock = std::chrono::steady_clock;
    auto time { Clock::now() };
    return std::chrono::duration_cast<std::chrono::duration<double>>
        (time.time_since_epoch()).count();
}

std::size_t ClockCallable::arity() {
    return 0;
}
// End ClockCallable

// Start FunctionCallable

std::any FunctionCallable::call(
    [[maybe_unused]] Interpreter& interpreter,
    [[maybe_unused]] std::vector<std::any> args
) {
    auto env { std::make_shared<Environment>(m_closure) };

    std::size_t i {};
    for (auto& decl : m_declaration->m_params)
        env->define(decl.m_lexeme, args[i++]);

    try {
        interpreter.executeBlock(m_declaration->m_body, env);
    }
    catch (const ReturnStmtStackError& returnValue){
        return (m_isInitialiser) ? m_closure->getAt(0, "this") : returnValue.m_value;
    }
    if (m_isInitialiser) return m_closure->getAt(0, "this");
    return {};
}

std::size_t FunctionCallable::arity() {
    return m_declaration->m_params.size();
}

std::string FunctionCallable::str() {
    return "<function " + m_declaration->m_name.m_lexeme + ">";
};

std::shared_ptr<FunctionCallable> FunctionCallable::bind(LoxInstance* instance) {
    auto env { std::shared_ptr<Environment>(m_closure) };
    // apparently use std::enable_shared_from_this ??
    env->define("this", std::shared_ptr<LoxInstance>(instance));
    return std::make_shared<FunctionCallable>(
        std::move(m_declaration), env, m_isInitialiser
    );
}

// End FunctionCallable