#include "Lox.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Lexer.h"

void Lox::runFile(const std::string& file) {
    std::ifstream ifs { file };
    if (!ifs)
        throw std::runtime_error("File: \'" + file + "\' not found.");

    std::stringstream ss;
    ss << ifs.rdbuf();

    run(ss.str());
    if (hadError) std::exit(EXIT_FAILURE);
}

void Lox::runPrompt() {
    std::cout << "Welcome to Lox REPL!\n";

    std::string line{};
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (!std::cin) break;

        run(line);
        hadError = false;
    }
}

void Lox::run(std::string_view src) {
    Lexer lex { src };
    lex.scanTokens();
}