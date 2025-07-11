#pragma once

#include <string_view>

namespace Errors {
    void errors(std::size_t line, std::string_view msg);
    void report(std::size_t line, std::string_view where, std::string_view msg);
}