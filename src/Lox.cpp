#include "Lox.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Lexer.h"
#include "Parser.h"
#include "AstPrinter.h"
#include "Interpreter.h"

void Lox::runFile(const std::string& file) {
    std::ifstream ifs { file };
    if (!ifs)
        throw std::runtime_error("File: \'" + file + "\' not found.");

    std::stringstream ss;
    ss << ifs.rdbuf();

    run(ss.str());
    if (hadError) std::exit(EXIT_FAILURE);
    if (hadRuntimeError) std::exit(EXIT_FAILURE); // change to appropriate exit code.
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
    auto tokens { lex.scanTokens() };

    Parser parser { tokens };
    auto expr { parser.parse() };

    if (hadError) {
        return;
    }

    AstPrinter printer {};
    std::cout << "*** Start printer details ***\n";
    std::cout << printer.print(expr.get());
    std::cout << "\n*** End printer details ***\n";

    Interpreter interpreter {};
    interpreter.interpret(expr.get());
}

void Lox::runtimeError(Interpreter::RuntimeError& error) {
    std::cout << error.what() << "\n [line " << error.m_token.m_line << "]\n";
    hadRuntimeError = true;
}