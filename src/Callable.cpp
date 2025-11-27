#include <chrono>

#include "Callable.h"
#include "Interpreter.h"

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

// Start Function

std::any FunctionCallable::call(
    [[maybe_unused]] Interpreter& interpreter,
    [[maybe_unused]] std::vector<std::any> args
) {
    std::shared_ptr<Environment> env { interpreter.m_globals };

    std::size_t i {};
    for (auto& decl : m_declaration->m_params)
        env->define(decl.m_lexeme, args[i++]);

    interpreter.executeBlock(m_declaration->m_body, env);
    return {};
}

std::size_t FunctionCallable::arity() {
    return m_declaration->m_params.size();
}

std::string FunctionCallable::str() {
    return "<function " + m_declaration->m_name.m_lexeme + ">";
};

// End Function