#pragma once

#include <any>
#include <vector>

#include "Interpreter.h"

class Callable {
public:
    std::any call(Interpreter interpreter, std::vector<std::any> args);
    std::size_t arity();
};