#include "shell.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>

void Shell::start_loop() {
    while (!should_exit) {
        char* input = readline("soshell> ");
        if (input && *input) {
            add_history(input);

            process_command(input);
            std::cout << "\n";

            free(input);
        }
    }

    return;
}

void Shell::process_command(std::string cmd) {
    if (cmd == "exit") {
        should_exit = true;
        return;
    } 

    if (cmd == "help") {
        std::cout << "Usage: my_function(\"arg1\", \"arg2\")\n\n";
        std::cout << "Commands:\n";
        std::cout << "   ls     List all symbols\n";
        std::cout << "   exit   Exit soshell\n";
        std::cout << "   help   Print this help\n";
        return;
    }

    if (cmd == "ls") {
        std::vector<std::string> symbols = library.list_symbols();

        for (auto& value : symbols) {
            std::cout << value << "\n";
        }

        return;
    }

    auto paren = cmd.find('(');
    auto close = cmd.rfind(')');

    if (paren == std::string::npos || close == std::string::npos) {
        std::cout << "Unknown command!\n";
        return;
    }

    std::string name = cmd.substr(0, paren);
    std::string args_str = cmd.substr(paren + 1, close - paren - 1);
    std::vector<std::string> args;
    std::stringstream ss(args_str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        auto start = token.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) continue;
        auto end = token.find_last_not_of(" \t\n\r");
        token = token.substr(start, end - start + 1);
        args.push_back(token);
    }

    library.call_symbol(name, args);
}

Shell::Shell(Library& lib) : library(lib) {
    library = lib;
}

