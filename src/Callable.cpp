#include <chrono>

#include "Callable.h"

// Begin ClockCallable

// get time in seconds? (double?) since epoch.
std::any ClockCallable::call(
    Interpreter interpreter, [[maybe_unused]] std::vector<std::any> args
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