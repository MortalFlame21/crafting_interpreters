# Crafting interpreters C++

A Lox interpreter following the _[Crafting Interpreters](https://craftinginterpreters.com/)_ book by Robert Nystrom. This
repository follows the first part of the book. This goal of this project is to get hands-on with compiler theory. Additionally, the project aims to improve C++ development, using C++17 features.

## Features

- [x] Expressions.
- [x] Variables (local, global).
- [x] Control flow (if, while, for statements).
- [x] Functions (recursion, closures).
- [x] Classes.
- [ ] Inheritance.

Please see `Parser.h.` for a high level understanding of the grammar in a loose high level understanding of Backus–Naur Form.

See `eg/` for a possible set of invalid and valid programs.

## Requirements

- CMake v3.15 or higher.
- C++17 compatible compiler.

## Usage

```
cmake -B .

make -C build/

./build/lox.out # To run the REPL
./build/lox.out <path_to_file> # To run file
```

## Improvements

- [ ] Unit testing.
- [ ] Error message improvement.
- [ ] Smart pointer usage improvement.
