#include <iostream>
#include "Lox.h"

int main(int argc, char* argv[]) {
    if (argc == 2) {
        Lox::runFile(argv[1]);
    }
    else if (argc == 1) {
        Lox::runPrompt();
    }
    else {
        std::cerr << "Usage ./jlox <source_file>\n";
        std::exit(EXIT_FAILURE);
    }
}